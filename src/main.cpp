#include <QCoreApplication>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>
#include <iostream>
#include <regex>
#include <iomanip>
#include <limits>
#include <unordered_map>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include "tinyxml2.h"
#include "avlicensing.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <future>
#include "opcuaclient.h"
#include <sys/statfs.h>

std::atomic<bool> running{true};
std::mutex result_mutex;

struct RaidStatus {
    bool exists;    // state: false/true
    std::string status; // "optimal", "degraded" или "rebuilding (X%)"
};

struct Memory {
    float total;
    float used;
    float free;
    short used_percent;
};

struct CpuTimes {
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    unsigned long long total() const {
        return user + nice + system + idle + iowait + irq + softirq + steal;
    }
};

// Чтение времени CPU
std::vector<CpuTimes> read_cpu_times() {
    std::ifstream file("/proc/stat");
    std::string line;
    std::vector<CpuTimes> times;
    while (std::getline(file, line)) { // читает построчно и сохраняет в line
        if (line.compare(0, 3, "cpu") != 0 || line.compare(0, 4, "cpu ") == 0)
            continue;
        std::istringstream ss(line);
        std::string cpu;
        CpuTimes t = {};
        ss >> cpu >> t.user >> t.nice >> t.system >> t.idle >> t.iowait >> t.irq >> t.softirq >> t.steal;
        times.push_back(t); // закидываем в вектор
    }
    return times;
}

// Получение списка точек монтирования из /proc/mounts
std::vector<std::string> get_mount_points() {
    std::vector<std::string> mount_points;
    std::ifstream mounts("/proc/mounts");
    if (!mounts.is_open()) {
        return mount_points;
    }

    std::string line;
    while (std::getline(mounts, line)) {
        std::istringstream iss(line);
        std::string device, mount_point, fs_type, options;
        if (iss >> device >> mount_point >> fs_type >> options) {
            mount_points.push_back(mount_point);
        }
    }
    mounts.close();
    return mount_points;
}


std::map<std::string, std::vector<int>> GetRunningProcesses() {
    std::map<std::string, std::vector<int>> processes;

    const char* cmd = "ps -e -o pid= -o comm=";
    std::array<char, 256> buffer;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        return processes;
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        std::string line(buffer.data());
        // line будет вида: "  1234 process\n"
        size_t pos = line.find_first_not_of(' ');
        if (pos == std::string::npos) continue;

        size_t space_pos = line.find(' ', pos);
        if (space_pos == std::string::npos) continue;

        std::string pid_str = line.substr(pos, space_pos - pos);
        std::string proc_name = line.substr(space_pos + 1);
        proc_name.erase(proc_name.find_last_not_of(" \n\r\t") + 1);

        int pid = std::stoi(pid_str);
        processes[proc_name].push_back(pid);
    }

    return processes;
}


// ------------------------------------------------------------------------

class IpChecker {
    std::vector<std::string> ips;
    std::unordered_map<std::string, bool> ip_status;
    std::vector<std::pair<std::string, std::shared_future<bool>>> async_checks;
    std::vector<std::string> failed_ips;

public:

    const std::unordered_map<std::string, bool>& get_status_map() const {
            return ip_status;
        }

    const std::vector<std::string>& get_failed_ips() const {
            return failed_ips;
        }

    void load_ips_from_xml() {
        async_checks.clear();
        ips.clear();
        ip_status.clear();
        failed_ips.clear();

        tinyxml2::XMLDocument doc;
        if (doc.LoadFile("config.xml") == tinyxml2::XML_SUCCESS) {
        auto* root = doc.RootElement();

        auto* av_diag = root->FirstChildElement("AvDiagnostics");

        for (auto* ips_xml = av_diag->FirstChildElement("IPs");
             ips_xml;
             ips_xml = ips_xml->NextSiblingElement("IPs")) {

            for (auto* ip = ips_xml->FirstChildElement("IP"); ip; ip = ip->NextSiblingElement("IP")) {
                if (const char* addr = ip->Attribute("address")) {
                    ips.push_back(addr);
                    ip_status[addr] = false;
                }
            }
        }
    }
    }

    void check_ips() {

        // Синхронная проверка
        for (const auto& addr : ips) {
            int status = system(("timeout 0.01 ping -c 1 " + addr + " >/dev/null 2>&1").c_str());
            bool connected = (status == 0);
            ip_status[addr] = connected;

            auto failed_it = std::find(failed_ips.begin(), failed_ips.end(), addr);
            if (!connected && failed_it == failed_ips.end()) {
                failed_ips.push_back(addr);
            } else if (connected && failed_it != failed_ips.end()) {
                failed_ips.erase(failed_it);
            }
        }

        for (const auto& addr : failed_ips) {
            bool already_checking = std::any_of(
                async_checks.begin(),
                async_checks.end(),
                [&addr](const auto& item) {
                    return item.first == addr;
                }
            );

            if (!already_checking) {
                auto future = std::async(std::launch::async, [addr] {
                    return system(("ping -c 1 -W 1 " + addr + " >/dev/null 2>&1").c_str()) == 0;
                });
                async_checks.emplace_back(addr, future.share());
            }
        }
    }

    void update_async_results() {

        auto it = async_checks.begin();
        while (it != async_checks.end()) {
            if (it->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                ip_status[it->first] = it->second.get();
                it = async_checks.erase(it); // деструктор не блокирует
            } else {
                ++it;
            }
        }
    }
};

// ===================================================================================


std::vector<std::string> ExtractProductKeysFromXml(const std::string& xml_path) {
    std::vector<std::string> products;
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(xml_path.c_str()) != tinyxml2::XML_SUCCESS) {
        return products;
    }
    auto* root = doc.FirstChildElement("AVDiagnostics");
    if (!root) return products;
    auto* grdkeys = root->FirstChildElement("GRDKeys");
    if (!grdkeys) return products;

    for (auto* key = grdkeys->FirstChildElement("Key"); key; key = key->NextSiblingElement("Key")) {
        const char* prod = key->Attribute("product");
        if (prod) products.emplace_back(prod);
    }
    return products;
}

std::vector<std::string> ExtractAllProductNames(const char* json);

// ----------------- CPU --------------------

struct CPUConfig {
    std::map<std::string, std::string> cpu_nodes; // key: "CPU0", "CPU1", etc., value: node_id from XML
    std::string type = "INT"; // default type
};

CPUConfig cpu_config;
std::map<std::string, int> last_sent_cpu_usages;

std::vector<double> get_cpu_usage() {
    auto t1 = read_cpu_times();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    auto t2 = read_cpu_times();

    std::vector<double> usages;
    for (size_t i = 0; i < t1.size(); ++i) {
        unsigned long long idle = t2[i].idle - t1[i].idle; // время простоя разница
        unsigned long long total = t2[i].total() - t1[i].total(); // разница общего времени
        double usage = 0.0;
        if (total != 0)
            usage = 100.0 * (1.0 - (double)idle / total); // вычисляем занятость
        usages.push_back(usage); // добавляем в вектор
    }
    return usages;
}

bool load_cpu_config(const std::string& config_path) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(config_path.c_str()) != tinyxml2::XML_SUCCESS) {
        return false;
    }

    auto* root = doc.FirstChildElement("root");

    auto* av_diag = root->FirstChildElement("AvDiagnostics");

    auto* cpus = av_diag->FirstChildElement("CPUs");

    for (auto* cpu = cpus->FirstChildElement("CPU"); cpu; cpu = cpu->NextSiblingElement("CPU")) {
        const char* number = cpu->Attribute("number");
        const char* node = cpu->Attribute("node");
        const char* type = cpu->Attribute("type");

        if (number && node) {
            std::string cpu_name = "CPU" + std::string(number);
            cpu_config.cpu_nodes[cpu_name] = node;
            if (type) cpu_config.type = type;
        }
    }

    return !cpu_config.cpu_nodes.empty();
}

std::map<std::string, int> collect_cpu_usages(const std::string& config_path) {
    static bool config_loaded = false;
    if (!config_loaded) {
        if (load_cpu_config(config_path)) {
            config_loaded = true;
        } else {
            return {};
        }
    }

    auto cpu_usages_vec = get_cpu_usage();
    std::map<std::string, int> current_usages;

    for (size_t i = 0; i < cpu_usages_vec.size(); ++i) {
        std::string cpu_name = "CPU" + std::to_string(i);
        if (cpu_config.cpu_nodes.find(cpu_name) != cpu_config.cpu_nodes.end()) {
            current_usages[cpu_name] = static_cast<int>(cpu_usages_vec[i]);
        }
    }

    return current_usages;
}

bool detect_cpu_changes(const std::map<std::string, int>& current_usages) {

    const int threshold = 1;

    if (last_sent_cpu_usages.size() != current_usages.size()) {
        return true;
    }

    for (const auto& [cpu, usage] : current_usages) {
        auto it = last_sent_cpu_usages.find(cpu);
        if (it == last_sent_cpu_usages.end() ||
            abs(it->second - usage) > threshold) {
            return true;
        }
    }

    return false;
}

void send_cpu_metrics(OpcUaClient& opcClient, const std::map<std::string, int>& current_usages) {

    std::vector<OpcUaClient::WriteConfig> writeConfigs;

    for (const auto& [cpu_name, usage] : current_usages) {
        auto node_it = cpu_config.cpu_nodes.find(cpu_name);
        if (node_it != cpu_config.cpu_nodes.end()) {
            OpcUaClient::WriteConfig config;

            config.allowed = true;
            config.node_id = node_it->second;
            config.type = cpu_config.type;

            if (cpu_config.type == "STRING") {
                config.value.s = std::to_string(usage);
            } else if (cpu_config.type == "INT") {
                config.value.i = usage;
            } else {
                config.value.f = static_cast<float>(usage);
            }

            writeConfigs.push_back(config);
        }
    }

    if (!writeConfigs.empty()) {
           opcClient.Write(writeConfigs);
           last_sent_cpu_usages = current_usages;
       }
}

// --------------- ДИСКИ ------------------------

struct DiskUsageData {
    uint64_t total = 0;
    uint64_t used = 0;
    uint64_t available = 0;
    int use_percent = 0;
    std::string file_system;
    std::string mount_point;
};

struct DiskConfig {
    std::string node_total;
    std::string type_total;
    std::string node_units_total;
    std::string type_units_total;
    std::string units_total;
    int precision_total;

    std::string node_used;
    std::string type_used;
    std::string node_units_used;
    std::string type_units_used;
    std::string units_used;
    int precision_used;

    std::string node_available;
    std::string type_available;
    std::string node_units_available;
    std::string type_units_available;
    std::string units_available;
    int precision_available;

    std::string node_percent;
    std::string type_percent;

    std::string node_file_system;
    std::string type_file_system;

    std::string node_mount_point;
    std::string type_mount_point;
};

std::map<std::string, DiskConfig> disk_configs;
std::map<std::string, DiskUsageData> last_sent_disk_usage;

bool load_disk_configs(const std::string& config_path) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(config_path.c_str()) != tinyxml2::XML_SUCCESS) {
        return false;
    }

    auto* root = doc.FirstChildElement("root");

    auto* av_diag = root->FirstChildElement("AvDiagnostics");

    auto* disks = av_diag->FirstChildElement("Disks");

    for (auto* disk = disks->FirstChildElement("Disk"); disk; disk = disk->NextSiblingElement("Disk")) {
        DiskConfig cfg;
        const char* name = disk->Attribute("name");

        // Загрузка конфигурации для total
        if (disk->Attribute("node_total")) cfg.node_total = disk->Attribute("node_total");
        if (disk->Attribute("type_total")) cfg.type_total = disk->Attribute("type_total");
        if (disk->Attribute("node_units_total")) cfg.node_units_total = disk->Attribute("node_units_total");
        if (disk->Attribute("units_total")) cfg.units_total = disk->Attribute("units_total");
        if (disk->QueryIntAttribute("precision_total", &cfg.precision_total) != tinyxml2::XML_SUCCESS) {
            cfg.precision_total = 0;
        }

        // Загрузка конфигурации для used
        if (disk->Attribute("node_used")) cfg.node_used = disk->Attribute("node_used");
        if (disk->Attribute("type_used")) cfg.type_used = disk->Attribute("type_used");
        if (disk->Attribute("node_units_used")) cfg.node_units_used = disk->Attribute("node_units_used");
        if (disk->Attribute("units_used")) cfg.units_used = disk->Attribute("units_used");
        if (disk->QueryIntAttribute("precision_used", &cfg.precision_used) != tinyxml2::XML_SUCCESS) {
            cfg.precision_used = 0;
        }

        // Загрузка конфигурации для available
        if (disk->Attribute("node_available")) cfg.node_available = disk->Attribute("node_available");
        if (disk->Attribute("type_available")) cfg.type_available = disk->Attribute("type_available");
        if (disk->Attribute("node_units_available")) cfg.node_units_available = disk->Attribute("node_units_available");
        if (disk->Attribute("units_available")) cfg.units_available = disk->Attribute("units_available");
        if (disk->QueryIntAttribute("precision_available", &cfg.precision_available) != tinyxml2::XML_SUCCESS) {
            cfg.precision_available = 0;
        }

        // Загрузка конфигурации для percent
        if (disk->Attribute("node_percent")) cfg.node_percent = disk->Attribute("node_percent");
        if (disk->Attribute("type_percent")) cfg.type_percent = disk->Attribute("type_percent");

        if (disk->Attribute("node_file_system")) cfg.node_file_system = disk->Attribute("node_file_system");
        if (disk->Attribute("type_file_system")) cfg.type_file_system = disk->Attribute("type_file_system");

        if (disk->Attribute("node_mount_point")) cfg.node_mount_point = disk->Attribute("node_mount_point");
        if (disk->Attribute("type_mount_point")) cfg.type_mount_point = disk->Attribute("type_mount_point");

        disk_configs[name] = cfg;
    }

    return !disk_configs.empty();
}

std::string get_filesystem_type(long f_type) {
    static const std::map<long, std::string> fs_types = {
        {0xADF5, "ADFS"},
        {0xADFF, "AFFS"},
        {0x5346414F, "AFS"},
        {0x09041934, "ANON_INODE_FS"},
        {0x61847373, "AUTOFS"},
        {0x9123683E, "BTRFS"},
        {0x27E0EB, "CGROUP"},
        {0x63677270, "CGROUP2"},
        {0xFF534D42, "CIFS"},
        {0x73757245, "CODA"},
        {0x012FF7B7, "COH"},
        {0x28CD3D45, "CRAMFS"},
        {0x64626720, "DEBUGFS"},
        {0x1CD1, "DEVFS"},
        {0x1373, "DEVFS"},
        {0x9FA0, "DEVPTS"},
        {0x13661366, "ECRYPTFS"},
        {0xDEAD3577, "EFS"},
        {0x414A53, "EFS"},
        {0x137D, "EXT"},
        {0xEF51, "EXT2"},
        {0xEF53, "EXT4"},
        {0xF2F52010, "F2FS"},
        {0x65735546, "FUSE"},
        {0xBAD1DEA, "FUTEXFS"},
        {0x4244, "HFS"},
        {0x00C0FFEE, "HOSTFS"},
        {0xF995E849, "HPFS"},
        {0x958458F6, "HUGETLBFS"},
        {0x9660, "ISOFS"},
        {0x72B6, "JFFS2"},
        {0x3153464A, "JFS"},
        {0x137F, "MINIX"},
        {0x138F, "MINIX (30 char)"},
        {0x2468, "MINIX2"},
        {0x2478, "MINIX2 (30 char)"},
        {0x4D44, "MSDOS"},
        {0x564C, "NCP"},
        {0x6969, "NFS"},
        {0x3434, "NILFS"},
        {0x6E736673, "NSFS"},
        {0x5346544E, "NTFS"},
        {0x7461636F, "OCFS2"},
        {0x9FA1, "OPENPROM"},
        {0x002F, "QNX4"},
        {0x68191122, "QNX6"},
        {0x858458F6, "RAMFS"},
        {0x52654973, "REISERFS"},
        {0x7275, "ROMFS"},
        {0x73636673, "SECURITYFS"},
        {0xF97CFF8C, "SELINUX"},
        {0x517B, "SMB"},
        {0x534F434B, "SOCKFS"},
        {0x73717368, "SQUASHFS"},
        {0x62656572, "SYSFS"},
        {0x012FF7B6, "SYSV2"},
        {0x012FF7B5, "SYSV4"},
        {0x01021994, "TMPFS"},
        {0x15013346, "UDF"},
        {0x00011954, "UFS"},
        {0x9FA2, "USBDEVICE"},
        {0x012FF7B4, "V7"},
        {0xA501FCF5, "VXFS"},
        {0xABBA1974, "XENFS"},
        {0x012FF7B8, "XFS"},
        {0x58465342, "XIAFS"}
    };

    auto it = fs_types.find(f_type);
    return it != fs_types.end() ? it->second : "unknown";
}

std::map<std::string, DiskUsageData> collect_disk_usage(const std::string& config_path = "config.xml") {
    std::map<std::string, DiskUsageData> current_data;

    // Загружаем конфигурацию при первом вызове
    static bool config_loaded = false;
    if (!config_loaded) {
        if (load_disk_configs(config_path)) {
            config_loaded = true;
        } else {
            return current_data;
        }
    }

    for (const auto& [name, _] : disk_configs) {
        struct statvfs stat;
        if (statvfs(name.c_str(), &stat) != 0) {
            continue;
        }

        DiskUsageData data;
        data.total = stat.f_blocks * stat.f_frsize;
        data.available = stat.f_bavail * stat.f_frsize;
        data.used = data.total - data.available;
        data.use_percent = (data.total == 0) ? 0 :
                          static_cast<int>((data.used * 100) / data.total);


        struct statfs fs_info;
                if (statfs(name.c_str(), &fs_info) == 0) {
                    data.file_system = get_filesystem_type(fs_info.f_type);
                } else {
                    data.file_system = "unknown";
                    std::cerr << "Ошибка statfs для " << name << ": " << strerror(errno) << std::endl;
                }

                data.mount_point = name;

        current_data[name] = data;
    }

    return current_data;
}

bool has_disk_changed(const DiskUsageData& old_data, const DiskUsageData& new_data) {
    return (old_data.use_percent != new_data.use_percent ||
            old_data.used != new_data.used ||
            old_data.total != new_data.total);
}

std::vector<std::pair<std::string, DiskUsageData>> find_changed_disks(
    const std::map<std::string, DiskUsageData>& current_data)
{
    std::vector<std::pair<std::string, DiskUsageData>> changed;

    for (const auto& [mp, data] : current_data) {
        auto it = last_sent_disk_usage.find(mp);

        if (it == last_sent_disk_usage.end()) {

            // Новый диск
            changed.emplace_back(mp, data);
            last_sent_disk_usage[mp] = data;
        } else if (has_disk_changed(it->second, data)) {

            // Данные изменились
            changed.emplace_back(mp, data);
            last_sent_disk_usage[mp] = data;
        }
    }

    return changed;
}

double convert_units(uint64_t bytes, const std::string& units, int precision) {
    double result = static_cast<double>(bytes);

    std::cout << "\nbytes: " << bytes << " units: " << units << " preci: " << precision;
    if (units == "ГБ") {
        result /= (1024.0 * 1024 * 1024);
    } else if (units == "МБ") {
        result /= (1024.0 * 1024);
    } else if (units == "КБ") {
        result /= 1024.0;
    }

    double multiplier = pow(10, precision);
    result = round(result * multiplier) / multiplier;
    return result;
}


void send_disk_metric(OpcUaClient& opcClient,
                     const std::string& node_id,
                     const std::string& type,
                     const float& value,
                     std::vector<OpcUaClient::WriteConfig>& writeConfigs) {
    OpcUaClient::WriteConfig cfg;
    cfg.allowed = true;
    cfg.node_id = node_id;
    cfg.type = type;

    if (type == "STRING") {
        cfg.value.s = std::to_string(value);
    } else if (type == "INT") {
        cfg.value.i = value;
    } else {
        cfg.value.f = static_cast<double>(value);
    }

    writeConfigs.push_back(cfg);
}

void send_full_disk_metrics(
    OpcUaClient& opcClient,
    const std::vector<std::pair<std::string, DiskUsageData>>& changed_disks
) {
    if (changed_disks.empty()) {
        return;
    }
    std::cout << "\nмы прошли?\n";

    std::vector<OpcUaClient::WriteConfig> writeConfigs;

    for (const auto& [mp, data] : changed_disks) {
        auto it = disk_configs.find(mp);
        if (it == disk_configs.end()) {
            continue;
        }

        const auto& cfg = it->second;

        if (!cfg.node_total.empty()) {
            send_disk_metric(opcClient, cfg.node_total, cfg.type_total, convert_units(data.total, cfg.units_total, cfg.precision_total), writeConfigs);

            if (!cfg.node_units_total.empty()) {
                OpcUaClient::WriteConfig units_cfg;
                units_cfg.allowed = true;
                units_cfg.node_id = cfg.node_units_total;
                units_cfg.type = "STRING";
                units_cfg.value.s = cfg.units_total;
                writeConfigs.push_back(units_cfg);
            }
        }

        if (!cfg.node_used.empty()) {
            send_disk_metric(opcClient, cfg.node_used, cfg.type_used, convert_units(data.used, cfg.units_used, cfg.precision_used), writeConfigs);

            if (!cfg.node_units_used.empty()) {
                OpcUaClient::WriteConfig units_cfg;
                units_cfg.allowed = true;
                units_cfg.node_id = cfg.node_units_used;
                units_cfg.type = "STRING";
                units_cfg.value.s = cfg.units_used;
                writeConfigs.push_back(units_cfg);
            }
        }

        if (!cfg.node_available.empty()) {
            send_disk_metric(opcClient, cfg.node_available, cfg.type_available, data.available, writeConfigs);

            if (!cfg.node_units_available.empty()) {
                OpcUaClient::WriteConfig units_cfg;
                units_cfg.allowed = true;
                units_cfg.node_id = cfg.node_units_available;
                units_cfg.type = "STRING";
                units_cfg.value.s = cfg.units_available;
                writeConfigs.push_back(units_cfg);
            }
        }

        if (!cfg.node_file_system.empty() && !data.file_system.empty()) {
                OpcUaClient::WriteConfig fs_cfg;
                fs_cfg.allowed = true;
                fs_cfg.node_id = cfg.node_file_system;
                fs_cfg.type = cfg.type_file_system;
                fs_cfg.value.s = data.file_system;
                writeConfigs.push_back(fs_cfg);
             }


        if (!cfg.node_mount_point.empty()) {
                OpcUaClient::WriteConfig mp_cfg;
                mp_cfg.allowed = true;
                mp_cfg.node_id = cfg.node_mount_point;
                mp_cfg.type = cfg.type_mount_point;
                mp_cfg.value.s = data.mount_point;
                writeConfigs.push_back(mp_cfg);
                }

        if (!cfg.node_percent.empty()) {
            send_disk_metric(opcClient, cfg.node_percent, cfg.type_percent, data.use_percent, writeConfigs);
        }
    }

    std::cout << "\nwriteconfigs пустой?" << writeConfigs.empty();
    std::cout << "\nА подключение? " << opcClient.CheckConnection();
    if (!writeConfigs.empty()) {
        opcClient.Write(writeConfigs);
    }
}


// ----------------- RAM -------------------
struct RAMState {
    unsigned long long total_kb = 0;
    unsigned long long used_kb = 0;
    unsigned long long free_kb = 0;
    std::time_t last_update;
};

struct RAMConfig {
    std::string node_total;
    std::string type_total;
    int precision_total = 2;

    std::string node_used;
    std::string type_used;
    int precision_used = 2;

    std::string node_free;
    std::string type_free;
    int precision_free = 2;
};

RAMConfig ram_config;
RAMState last_sent_ram;
std::chrono::steady_clock::time_point last_ram_send;

// 1. Загрузка конфигурации RAM
bool load_ram_config(const std::string& config_path) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(config_path.c_str()) != tinyxml2::XML_SUCCESS) {

        return false;
    }

    auto* root = doc.FirstChildElement("root");
    if (!root) return false;

    auto* av_diag = root->FirstChildElement("AvDiagnostics");
    if (!av_diag) return false;

    auto* ram = av_diag->FirstChildElement("RAM");
    if (!ram) return false;

    // Загрузка TotalRAM
    if (auto* total = ram->FirstChildElement("TotalRAM")) {
        ram_config.node_total = total->Attribute("node");
        ram_config.type_total = total->Attribute("type");
        total->QueryIntAttribute("precision", &ram_config.precision_total);
    }

    // Загрузка UsedRAM
    if (auto* used = ram->FirstChildElement("UsedRAM")) {
        ram_config.node_used = used->Attribute("node") ? used->Attribute("node") : "";
        ram_config.type_used = used->Attribute("type") ? used->Attribute("type") : "REAL";
        used->QueryIntAttribute("precision", &ram_config.precision_used);
    }

    // Загрузка FreeRAM
    if (auto* free = ram->FirstChildElement("FreeRAM")) {
        ram_config.node_free = free->Attribute("node") ? free->Attribute("node") : "";
        ram_config.type_free = free->Attribute("type") ? free->Attribute("type") : "REAL";
        free->QueryIntAttribute("precision", &ram_config.precision_free);
    }

    return true;
}

// 2. Сбор данных о RAM
RAMState collect_ram_usage() {
    RAMState state;
    std::array<char, 128> buffer;
    std::string line;

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("free -k | grep Mem:", "r"), pclose);
    if (!pipe) return state;

    if (fgets(buffer.data(), buffer.size(), pipe.get())) {
        line = buffer.data();
        std::istringstream iss(line);
        std::string label;
        float shared_kb, buff_cache_kb;
        iss >> label >> state.total_kb >> state.used_kb >> state.free_kb
            >> shared_kb >> buff_cache_kb; // Эти поля больше не используются
    }

    state.last_update = std::time(nullptr);
    return state;
}

// 3. Проверка изменений (с порогом 1MB)
bool is_ram_changed(const RAMState& current) {
    const unsigned long long threshold_kb = 1024; // 1MB

    return
        std::abs(static_cast<long>(current.total_kb - last_sent_ram.total_kb)) > threshold_kb ||
        std::abs(static_cast<long>(current.used_kb - last_sent_ram.used_kb)) > threshold_kb ||
        std::abs(static_cast<long>(current.free_kb - last_sent_ram.free_kb)) > threshold_kb;
}

// 4. Отправка метрик в OPC UA
void send_ram_metrics(OpcUaClient& opcClient, const RAMState& data) {
    std::vector<OpcUaClient::WriteConfig> writeConfigs;

    std::cout << "\ncfg: " << ram_config.node_total << " : " << ram_config.type_total;
    // Конвертация значений в ГБ (по умолчанию)
    auto convert_to_gb = [](unsigned long long kb, int precision) {
        return static_cast<float>(kb);
    };

    // Total RAM
    if (!ram_config.node_total.empty()) {
        OpcUaClient::WriteConfig cfg;
        cfg.allowed = true;
        cfg.node_id = ram_config.node_total;
        cfg.type = ram_config.type_total;

        float value = convert_to_gb(data.total_kb, ram_config.precision_total);
        if (ram_config.type_total == "INT") {
            cfg.value.i = static_cast<int>(value);
        } else {
            cfg.value.f = static_cast<float>(value);
        }
        writeConfigs.push_back(cfg);
    }

    // Used RAM
    if (!ram_config.node_used.empty()) {
        OpcUaClient::WriteConfig cfg;
        cfg.allowed = true;
        cfg.node_id = ram_config.node_used;
        cfg.type = ram_config.type_used;

        float value = convert_to_gb(data.used_kb, ram_config.precision_used);
        if (ram_config.type_used == "INT") {
            cfg.value.i = static_cast<int>(value);
        } else {
            cfg.value.f = static_cast<float>(value);
        }
        writeConfigs.push_back(cfg);
    }

    // Free RAM
    if (!ram_config.node_free.empty()) {
        OpcUaClient::WriteConfig cfg;
        cfg.allowed = true;
        cfg.node_id = ram_config.node_free;
        cfg.type = ram_config.type_free;

        float value = convert_to_gb(data.free_kb, ram_config.precision_free);
        if (ram_config.type_free == "INT") {
            cfg.value.i = static_cast<int>(value);
        } else {
            cfg.value.f = static_cast<float>(value);
        }
        writeConfigs.push_back(cfg);
    }

    //std::cout << "\n node: " << writeConfigs[0].node_id << "\n type" << writeConfigs[0].type << "\n value" << writeConfigs[0].value.f;
    std::cout << "\nПусто RAM " << writeConfigs.empty() << " | " << !opcClient.CheckConnection();
    if (!writeConfigs.empty()) {
        opcClient.Write(writeConfigs);
        last_sent_ram = data;
    }
}


// ---------------- Processes ---------------------
struct ProcessState {
    std::string status;
    bool is_group_result = false;
    std::string group_node;
    std::string value_type;
};

std::map<std::string, ProcessState> last_sent_states;

// 1. Сбор состояний процессов
std::map<std::string, ProcessState> collect_process_states(const std::string& xml_path) {
    std::map<std::string, ProcessState> current_states;
    tinyxml2::XMLDocument doc;

    if (doc.LoadFile(xml_path.c_str()) == tinyxml2::XML_SUCCESS) {
        if (auto* root = doc.FirstChildElement("root")) {
            if (auto* av_diag = root->FirstChildElement("AvDiagnostics")) {
                // Получаем список всех существующих процессов один раз
                auto running_processes = GetRunningProcesses();

                for (auto* processes = av_diag->FirstChildElement("Processes");
                     processes;
                     processes = processes->NextSiblingElement("Processes")) {

                    const char* group_node = processes->Attribute("node");
                    const char* logic = processes->Attribute("logic");
                    const char* type = processes->Attribute("type");

                    if (!group_node || !type || (logic && std::string(logic) != "AND"))
                        continue;

                    std::vector<std::string> members;
                    for (auto* proc = processes->FirstChildElement("Process");
                         proc;
                         proc = proc->NextSiblingElement("Process")) {

                        if (const char* name = proc->Attribute("name")) {
                            members.push_back(name);
                        }
                    }

                    if (!members.empty()) {
                        bool all_running = true;
                        bool all_exist = true;

                        for (const auto& name : members) {
                            auto it = running_processes.find(name);
                            bool exists = (it != running_processes.end());
                            bool is_running = exists && !it->second.empty(); // Процесс работает, если есть хотя бы один PID

                            ProcessState state;
                            state.status = exists ? (is_running ? "RUNNING" : "NOT_RUNNING") : "NOT_EXIST";
                            current_states[name] = state;

                            if (!exists) {
                                all_exist = false;
                                all_running = false;
                            } else if (!is_running) {
                                all_running = false;
                            }
                        }

                        // Группа считается работающей только если ВСЕ процессы:
                        // 1. Существуют в системе
                        // 2. Находятся в состоянии RUNNING
                        ProcessState group_state;
                        group_state.status = (all_exist && all_running) ? "RUNNING" : "NOT_RUNNING";
                        group_state.is_group_result = true;
                        group_state.group_node = group_node;
                        group_state.value_type = type;
                        current_states[group_node] = group_state;
                    }
                }
            }
        }
    }

    return current_states;
}

// 2. Обнаружение изменений
bool detect_process_changes(const std::map<std::string, ProcessState>& current) {
    // Проверяем только групповые ноды
    for (const auto& [name, state] : current) {
        if (!state.is_group_result) continue;

        auto it = last_sent_states.find(name);
        if (it == last_sent_states.end() || it->second.status != state.status) {
            return true;
        }
    }
    return false;
}

// 3. Отправка метрик в OPC
void send_process_metrics(OpcUaClient& opcClient,
                         const std::map<std::string, ProcessState>& current) {

    std::vector<OpcUaClient::WriteConfig> writes;

    for (const auto& [name, state] : current) {
        if (!state.is_group_result) continue;

        OpcUaClient::WriteConfig cfg;
        cfg.node_id = state.group_node;
        cfg.type = state.value_type;

        if (state.value_type == "BOOL" || state.value_type == "INT") {
            cfg.value.i = (state.status == "RUNNING") ? 1 : 0;
        }
        else if (state.value_type == "REAL") {
            cfg.value.f = (state.status == "RUNNING") ? 1.0f : 0.0f;
        }
        else {
            cfg.value.s = state.status;
        }

        writes.push_back(cfg);
        last_sent_states[name] = state;
    }

    if (!writes.empty() && opcClient.CheckConnection()) {
        opcClient.Write(writes);
    }
}

// ----------------- RAID -----------------------

struct RaidState {
    std::string status;      // "optimal", "degraded", "rebuilding", "inactive"
    bool state;              // исправность (true, false)
    double rebuild_percent;  // -1 если не в восстановлении
};

struct RaidNodes{
        std::string node_status;
        std::string type_status;
        std::string node_state;
        std::string type_state;
        std::string node_rebuild;
        std::string type_rebuild;
    };

struct RaidConfig {
    std::map<std::string, RaidNodes> raid_configs;
};

RaidConfig raid_config;
std::map<std::string, RaidState> last_sent_raids;

bool load_raid_config(const std::string& config_path) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(config_path.c_str()) != tinyxml2::XML_SUCCESS) {
        return false;
    }

    auto* root = doc.FirstChildElement("root");
    if (!root) return false;

    auto* av_diag = root->FirstChildElement("AvDiagnostics");
    if (!av_diag) return false;

    auto* raids = av_diag->FirstChildElement("RAIDs");
    if (!raids) return false;

    for (auto* raid = raids->FirstChildElement("RAID"); raid; raid = raid->NextSiblingElement("RAID")) {
        const char* name = raid->Attribute("name");
        if (!name) continue;

        raid_config.raid_configs[name] = {
            .node_status = raid->Attribute("node_status"),
            .type_status = raid->Attribute("type_status"),
            .node_state = raid->Attribute("node_state"),
            .type_state = raid->Attribute("type_state"),
        };
    }

    return !raid_config.raid_configs.empty();
}

std::map<std::string, RaidState> collect_raid_states(const std::string& config_path) {
    static bool config_loaded = false;
    if (!config_loaded) {
        if (load_raid_config(config_path)) {
            config_loaded = true;
        } else {
            return {};
        }
    }

    std::map<std::string, RaidState> current_states;
    std::ifstream mdstat("/proc/mdstat");

    if (!mdstat.is_open()) {
        RaidState state;
        state.status = "not_available";
        state.state = false;
        state.rebuild_percent = -1;
        current_states["system"] = state;
        return current_states;
    }

    std::string line;
    std::string current_raid;
    bool in_raid_section = false;

    // Регулярные выражения
    std::regex raid_regex(R"(^(md\d+)\s*:.*)");
    std::regex state_regex(R"(\[([U_/]+)\])");  // Состояние дисков: [UU], [U_], [3/3]
    std::regex rebuild_regex(R"((recovery|resync)\s*=\s*(\d+\.\d+)%)");
    std::regex active_regex(R"(active\s+(raid\d+))");

    while (std::getline(mdstat, line)) {
        std::smatch match;

        // Поиск начала секции RAID
        if (std::regex_search(line, match, raid_regex)) {
            current_raid = match[1];
            in_raid_section = true;

            // Проверяем, есть ли этот RAID в конфигурации
            if (raid_config.raid_configs.find(current_raid) == raid_config.raid_configs.end()) {
                current_raid.clear();
                in_raid_section = false;
                continue;
            }

            RaidState state;
            state.state = false;
            state.rebuild_percent = -1;

            // Проверка состояния дисков
            if (std::regex_search(line, match, state_regex)) {
                std::string disk_state = match[1];
                if (disk_state.find('_') != std::string::npos) {
                    state.status = "degraded";
                } else {
                    state.status = "optimal";
                    state.state = true;
                }
            } else {
                state.status = "unknown";
            }

            current_states[current_raid] = state;
            continue;
        }

        // Обработка секции восстановления
        if (in_raid_section && !current_raid.empty()) {
            if (std::regex_search(line, match, rebuild_regex)) {
                current_states[current_raid].rebuild_percent = std::stod(match[2]);
                current_states[current_raid].status = "rebuilding " + std::string(match[2]) + "%";
                current_states[current_raid].state = false;
            }
        }

        // Сброс флага при пустой строке
        if (line.empty()) {
            in_raid_section = false;
            current_raid.clear();
        }
    }

    // Если RAID-массивы найдены, но не обработаны
    for (const auto& [name, cfg] : raid_config.raid_configs) {
        if (current_states.find(name) == current_states.end()) {
            RaidState state;
            state.status = "inactive";
            state.state = false;
            state.rebuild_percent = -1;
            current_states[name] = state;
        }
    }

    return current_states;
}


std::map<std::string, RaidState> detect_raid_changes(
    const std::map<std::string, RaidState>& current) {

    static bool first_run = true;
    std::map<std::string, RaidState> changed;


    if (first_run) {
        last_sent_raids = current;
        first_run = false;
        return current;
    }


    // Поиск изменений
    for (const auto& [name, current_state] : current) {
        auto last_it = last_sent_raids.find(name);

        // Новый RAID
        if (last_it == last_sent_raids.end()) {
            changed[name] = current_state;
            continue;
        }

        // Сравнение состояний
        const auto& last_state = last_it->second;
        bool has_changes = false;

        if (last_state.status != current_state.status) {
            has_changes = true;
        }

        if (last_state.state != current_state.state) {
            has_changes = true;
        }

        if ((last_state.rebuild_percent >= 0 || current_state.rebuild_percent >= 0) &&
            std::abs(last_state.rebuild_percent - current_state.rebuild_percent) >= 0.1) {
            has_changes = true;
        }

        if (has_changes) {
            changed[name] = current_state;
        }
    }

    // Поиск удаленных RAID
    for (const auto& [name, _] : last_sent_raids) {
        if (current.find(name) == current.end()) {
            RaidState removed_state;
            removed_state.status = "removed";
            removed_state.state = false;
            removed_state.rebuild_percent = -1;
            changed[name] = removed_state;
        }
    }

    return changed;
}

void send_raid_metrics(OpcUaClient& opcClient,
    const std::map<std::string, RaidState>& changed) {

    if (changed.empty()) {std::cout << "\nизменения пустые у рейда"; return;}


    std::vector<OpcUaClient::WriteConfig> writeConfigs;

    for (const auto& [name, state] : changed) {
        std::cout << "\nnama " << name << "\n";
        auto config_it = raid_config.raid_configs.find(name);
        if (config_it == raid_config.raid_configs.end()) continue;

        // Статус RAID
        OpcUaClient::WriteConfig status_cfg;
        status_cfg.allowed = true;
        status_cfg.node_id = config_it->second.node_status;
        status_cfg.type = config_it->second.type_status;
        status_cfg.value.s = state.status;
        std::cout << "\nRaid Status node " << status_cfg.node_id << " | " << status_cfg.value.s;
        writeConfigs.push_back(status_cfg);

        // Исправность
        OpcUaClient::WriteConfig health_cfg;

        health_cfg.allowed = true;
        health_cfg.node_id = config_it->second.node_state;
        health_cfg.type = config_it->second.type_state;
            if (health_cfg.type == "STRING") {
                health_cfg.value.s = std::to_string(state.state);
            } else if (health_cfg.type == "BOOL") {
                health_cfg.value.b = state.state;
            } else {
                health_cfg.value.f = static_cast<float>(state.state);
            }


        std::cout << "\nRaid bool node " << health_cfg.node_id << " | " << health_cfg.value.i;
        writeConfigs.push_back(health_cfg);
        std::cout << "\n\nwriteconf: " << writeConfigs[0].node_id << ", " << writeConfigs[1].node_id;

//        // Процент восстановления
//        if (state.rebuild_percent >= 0 && !config_it->second.node_rebuild.empty()) {
//            OpcUaClient::WriteConfig rebuild_cfg;
//            rebuild_cfg.allowed = true;
//            rebuild_cfg.node_id = config_it->second.node_rebuild;
//            rebuild_cfg.type = config_it->second.type_rebuild;

//            if (config_it->second.type_rebuild == "INT") {
//                rebuild_cfg.value.i = static_cast<int>(state.rebuild_percent);
//            } else {
//                rebuild_cfg.value.f = state.rebuild_percent;
//            }

//            writeConfigs.push_back(rebuild_cfg);
//        }
    }

    if (!writeConfigs.empty()) {
        std::cout << "\nСЮда?";
        opcClient.Write(writeConfigs);

        for (const auto& [name, state] : changed) {
            if (state.status == "removed") {
                last_sent_raids.erase(name);
            } else {
                last_sent_raids[name] = state;
            }
        }
    }
}

// ------------------ IP ---------------------

struct IpState {
    bool connected;
    std::time_t last_change;
    bool is_group_result;
    std::string group_node;
    std::string value_type;
};

std::map<std::string, IpState> last_sent_ips;
IpChecker ip_checker;

// 1. Функция сбора состояний IP
// 1. Функция сбора состояний IP с поддержкой групп
std::map<std::string, IpState> collect_ip_states() {
    std::map<std::string, IpState> current_states;
    std::map<std::string, std::pair<std::vector<std::string>, std::string>> ip_groups;

    // Загрузка конфигурации IP групп
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile("config.xml") == tinyxml2::XML_SUCCESS) {
        auto* root = doc.FirstChildElement("root");
        if (root) {
            auto* av_diag = root->FirstChildElement("AvDiagnostics");
            if (av_diag) {
                for (auto* ips = av_diag->FirstChildElement("IPs"); ips; ips = ips->NextSiblingElement("IPs")) {
                    const char* logic = ips->Attribute("logic");
                    const char* group_node = ips->Attribute("node");
                    const char* group_type = ips->Attribute("type");

                    if (!group_node || !group_type) continue;

                    std::vector<std::string> group_ips;
                    for (auto* ip = ips->FirstChildElement("IP"); ip; ip = ip->NextSiblingElement("IP")) {
                        if (const char* address = ip->Attribute("address")) {
                            group_ips.push_back(address);
                        }
                    }

                    if (!group_ips.empty()) {
                        ip_groups[group_node] = {group_ips, logic ? logic : "AND"};
                    }
                }
            }
        }
    }

    ip_checker.load_ips_from_xml();
    ip_checker.check_ips();
    ip_checker.update_async_results();
    auto& status_map = ip_checker.get_status_map();

    // Формирование состояний отдельных IP
    for (const auto& [ip, connected] : status_map) {
        IpState state;
        state.connected = connected;
        state.last_change = std::time(nullptr);
        state.is_group_result = false;
        current_states[ip] = state;
    }

    // Проверка групп IP и формирование групповых состояний
    for (const auto& [group_node, group_data] : ip_groups) {
        const auto& [ips, logic] = group_data;
        bool group_result = (logic == "AND"); // Инициализация в зависимости от логики

        for (const auto& ip : ips) {
            auto it = status_map.find(ip);
            bool connected = (it != status_map.end() && it->second);

            if (logic == "AND") {
                group_result &= connected;
            } else { // OR
                group_result |= connected;
            }
        }

        // Сохраняем результат группы
        IpState group_state;
        group_state.connected = group_result;
        group_state.last_change = std::time(nullptr);
        group_state.is_group_result = true;
        group_state.group_node = group_node;
        group_state.value_type = "BOOL"; // Для IP групп всегда BOOL
        current_states[group_node] = group_state;
    }

    return current_states;
}

// 2. чек изменения
std::map<std::string, IpState> detect_ip_changes(
    const std::map<std::string, IpState>& current) {

    std::map<std::string, IpState> changed;

    if (last_sent_ips.empty()) {
            last_sent_ips = current;
            return current;
        }

    // Проверяем изменения в существующих IP
    for (const auto& [ip, current_state] : current) {
        auto last_it = last_sent_ips.find(ip);
        if (last_it == last_sent_ips.end()) {
            // Новый IP
            changed[ip] = current_state;
        } else if (last_it->second.connected != current_state.connected) {
            // Статус изменился
            changed[ip] = current_state;
        }
    }

    // Проверяем удаленные IP
    for (const auto& [ip, _] : last_sent_ips) {
        if (current.find(ip) == current.end()) {
            IpState removed_state;
            removed_state.connected = false;
            removed_state.last_change = std::time(nullptr);
            changed[ip] = removed_state;
        }
    }

    last_sent_ips = current;
    return changed;
}

// 3. Функция отправки в OPC UA
struct IpConfig {
    std::map<std::string, std::pair<std::string, std::string>> ip_configs; // <address, <node_id, type>>
};

IpConfig ip_config;


void send_ip_metrics(OpcUaClient& opcClient, const std::map<std::string, IpState>& changed) {
    std::vector<OpcUaClient::WriteConfig> writeConfigs;

    for (const auto& [name, state] : changed) {
        // Отправляем только групповые ноды
        if (!state.is_group_result) continue;

        OpcUaClient::WriteConfig cfg;
        cfg.allowed = true;
        cfg.node_id = state.group_node;
        cfg.type = state.value_type;

        if (state.value_type == "BOOL") {
            cfg.value.i = state.connected ? 1 : 0;
        } else {
            cfg.value.s = state.connected ? "true" : "false";
        }

        writeConfigs.push_back(cfg);
    }

    if (!writeConfigs.empty() && opcClient.CheckConnection()) {
        opcClient.Write(writeConfigs);

        // Обновляем last_sent_ips только для групповых нод
        for (const auto& [name, state] : changed) {
            if (state.is_group_result) {
                last_sent_ips[name] = state;
            }
        }
    }
}

// --------------- Guardant ------------------

struct GuardantProductState {
    bool found;              // Найден ли ключ продукта
    std::time_t last_check;
};

struct GuardantState {
    bool key_inserted;
    std::map<std::string, GuardantProductState> products; // Состояние продуктов
};

GuardantState last_sent_guardant;
const std::chrono::seconds guardant_check_interval(30);

// 1. Функция сбора состояния Guardant
GuardantState collect_guardant_states(const std::string& xml_path) {
    GuardantState current_state;
    AVLicensing licensing;
    tinyxml2::XMLDocument doc;
    std::time_t now = std::time(nullptr);

    // 1. Проверка физического ключа
    int ret = system("lsusb | grep -iq Aktiv");
    current_state.key_inserted = (ret == 0);

    // 2. Загрузка XML
    if (doc.LoadFile(xml_path.c_str()) != tinyxml2::XML_SUCCESS) {
        return current_state;
    }

    // 3. Получение информации о лицензиях
    std::string licenseJson = licensing.GetLicenseInfoJson();
    std::vector<std::string> licensedProducts;
    if (!licenseJson.empty()) {
        licensedProducts = ExtractAllProductNames(licenseJson.c_str());
    }

    // 4. Извлечение продуктов из XML
    auto* grd_keys = doc.FirstChildElement("root")
                   ->FirstChildElement("AvDiagnostics")
                   ->FirstChildElement("GRDKeys");

    if (!grd_keys) {
        return current_state;
    }

    // 5. Проверка состояния продуктов
    for (auto* key = grd_keys->FirstChildElement("Key"); key != nullptr; key = key->NextSiblingElement("Key")) {
        const char* product = key->Attribute("product");
        if (!product) continue;

        GuardantProductState product_state;
        product_state.last_check = now;

        if (!current_state.key_inserted) {
            product_state.found = false;
        } else if (licenseJson.empty()) {
            product_state.found = false;
        } else {
            auto it = std::find_if(
                licensedProducts.begin(),
                licensedProducts.end(),
                [&product](const std::string& licProduct) {
                    return strcasecmp(product, licProduct.c_str()) == 0;
                }
            );
            product_state.found = (it != licensedProducts.end());
        }

        current_state.products[product] = product_state;
    }

    return current_state;
}

GuardantState detect_guardant_changes(const GuardantState& current) {
    GuardantState changed;

    // Проверка физического ключа
    if (current.key_inserted != last_sent_guardant.key_inserted) {
        changed.key_inserted = current.key_inserted;
    }

    // Проверка продуктов
    for (const auto& [product, current_state] : current.products) {
        auto last_it = last_sent_guardant.products.find(product);

        // Новый продукт или изменился статус
        if (last_it == last_sent_guardant.products.end() ||
            last_it->second.found != current_state.found) {
            changed.products[product] = current_state;
        }
    }

    // Проверка удаленных продуктов
    for (const auto& [product, _] : last_sent_guardant.products) {
        if (current.products.find(product) == current.products.end()) {
            GuardantProductState removed_state;
            removed_state.found = false;
            removed_state.last_check = std::time(nullptr);
            changed.products[product] = removed_state;
        }
    }

    return changed;
}

// 3. Функция отправки в OPC UA
void send_guardant_metrics(OpcUaClient& opcClient,
                          const GuardantState& changed,
                          const std::string& config_path) {
    if (changed.products.empty() || !changed.key_inserted) return;

    // Загрузка конфигурации из XML
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(config_path.c_str()) != tinyxml2::XML_SUCCESS) {
        return;
    }

    // Поиск секции GRDKeys в XML
    auto* grd_keys = doc.FirstChildElement("root")
                   ->FirstChildElement("AvDiagnostics")
                   ->FirstChildElement("GRDKeys");
    if (!grd_keys) {
        return;
    }

    std::vector<OpcUaClient::WriteConfig> writeConfigs;
    const auto ts = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    const uint32_t qual = UA_STATUSCODE_GOOD;

    for (const auto& [product, state] : changed.products) {


        bool key_found = false;
        for (auto* key = grd_keys->FirstChildElement("Key"); key != nullptr; key = key->NextSiblingElement("Key")) {
            const char* xml_product = key->Attribute("product");
            if (xml_product && product == xml_product) {

                //std::cout << "\nОтправка grd";
                OpcUaClient::WriteConfig status_cfg;
                status_cfg.allowed = true;
                status_cfg.node_id = key->Attribute("node");
                status_cfg.type = key->Attribute("type");
                status_cfg.value.i = state.found;

                writeConfigs.push_back(status_cfg);
                key_found = true;
                break;
            }
        }
    }

    // Отправка данных в OPC UA
    if (!writeConfigs.empty() && opcClient.CheckConnection()) {
        opcClient.Write(writeConfigs);

        // Обновление кэша
        if (changed.key_inserted) {
            last_sent_guardant.key_inserted = changed.key_inserted;
        }

        for (const auto& [product, state] : changed.products) {
            if (!state.found && state.last_check > 0) {
                last_sent_guardant.products.erase(product);
            } else {
                last_sent_guardant.products[product] = state;
            }
        }
    }
}


// ----------------- main ----------------------


// Вспомогательная функция для интервалов
bool time_passed(std::chrono::steady_clock::time_point& last, std::chrono::seconds interval) {
    auto now = std::chrono::steady_clock::now();
    if (now - last >= interval) {
        last = now;
        return true;
    }
    return false;
}

int main() {
    // Инициализация временных меток
    auto last_fast = std::chrono::steady_clock::now();
    auto last_opc_send = last_fast;
    auto last_guardant_start = last_fast;

    // Интервалы
    const std::chrono::seconds fast_interval(1);
    const std::chrono::seconds guardant_interval(10);

    // Асинхронные задачи
    std::future<GuardantState> guardant_future;
    std::atomic<bool> guardant_check_running{false};

    // Контейнеры данных
    // Загрузка конфигурации OPC из XML
    const std::string config_path = "config.xml";
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(config_path.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Ошибка загрузки файла конфигурации" << std::endl;
        return 1;
    }

    // Получение параметров OPC-сервера
    std::string opc_address = "192.168.6.72";
    int opc_port = 62544;

    if (auto* root = doc.FirstChildElement("root")) {
        if (auto* av_diag = root->FirstChildElement("AvDiagnostics")) {
            if (auto* opc = av_diag->FirstChildElement("OPC")) {
                if (auto* server = opc->FirstChildElement("Server")) {
                    if (const char* address = server->Attribute("address")) {
                        if (const char* port = server->Attribute("port")) {
                            opc_port = std::stoi(port);
                            opc_address = address;
                        }
                    }
                }
            }
        }
    }

    // Инициализация OPC-клиента с параметрами из XML
    OpcUaClient opcClient(opc_address, opc_port);
    std::cout << "\nOPC "
              << opc_address << ":" << opc_port << std::endl;
    IpChecker ip_checker;
    ip_checker.load_ips_from_xml();



    // Создание переменных для grd
    doc.LoadFile(config_path.c_str());

    while (true) {

        auto now = std::chrono::steady_clock::now();

        // ============= БЫСТРЫЕ ПРОВЕРКИ (1 сек) =============
        if (time_passed(last_fast, fast_interval)) {

            // Обновление IP статусов
            ip_checker.load_ips_from_xml();
            ip_checker.check_ips();
        }



        // ============= ОТПРАВКА В OPC UA (1 сек) =============
        if (time_passed(last_opc_send, fast_interval)) {
            // CPU ----------------------------

            auto current_cpu_usages = collect_cpu_usages(config_path);
            if (detect_cpu_changes(current_cpu_usages)) {
                std::cout << "\n CPU не отправляем (нет нод)";
                send_cpu_metrics(opcClient, current_cpu_usages);
            }

            // Disks ----------------------------
            auto disk_info = collect_disk_usage(config_path);
            auto changed_disks = find_changed_disks(disk_info);

            std::cout << "\nПусто ли в дисках: " << changed_disks.empty();
            if (!changed_disks.empty()) {
                std::cout << "\nотправка disks";
                send_full_disk_metrics(opcClient, changed_disks);
            }

            // RAM  ----------------------------

            load_ram_config(config_path);
            RAMState current_ram = collect_ram_usage();
            if (is_ram_changed(current_ram)) {
                std::cout << "\nотправка ram";
                send_ram_metrics(opcClient, current_ram);
            }

            // Processes -----------------------

            auto current_processes = collect_process_states(config_path);
            if (detect_process_changes(current_processes)) {
                std::cout << "\nотправка processes";

                send_process_metrics(opcClient, current_processes);
            }

            // RAID ----------------------------

            auto current_raids = collect_raid_states(config_path);
            auto changed_raids = detect_raid_changes(current_raids);
            if (!changed_raids.empty()) {
                std::cout << "\nотправка RAID";
                send_raid_metrics(opcClient, changed_raids);
            }

            // IP  ----------------------------
            auto current_ips = collect_ip_states();

            auto changed_ips = detect_ip_changes(current_ips);
            if (!changed_ips.empty()) {
                std::cout << "\nотправка ip";
                send_ip_metrics(opcClient, changed_ips);
            }
        }

        // ============= GUARDANT (30 сек) =============
        // Запуск асинхронной проверки

        if (time_passed(last_guardant_start, guardant_interval) &&
            !guardant_check_running) {


            guardant_check_running = true;
            guardant_future = std::async(std::launch::async, [&] {
                return collect_guardant_states(config_path);
            });
        }

        // Обработка результатов Guardant
        if (guardant_check_running &&
            guardant_future.valid() &&
            guardant_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {

            try {
                GuardantState current_guardant = guardant_future.get();
                auto changed_guardant = detect_guardant_changes(current_guardant);
                    send_guardant_metrics(opcClient, changed_guardant, config_path);
                    changed_guardant = {};
            } catch (const std::exception& e) {
            }
            guardant_check_running = false;
        }

        // Задержка для снижения нагрузки
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    return 0;
}
