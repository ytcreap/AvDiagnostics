#include "avdiagnostics.h"



// -------- Методы --------------

int AvDiagnostics::loadXml()
{
    const std::string config_path = "config.xml";
    if (doc_.LoadFile(config_path.c_str()) != tinyxml2::XML_SUCCESS) {
        doc_.LoadFile(config_path.c_str()) ;
        return 1;
    }
    else
        return 0;
}

tinyxml2::XMLElement* AvDiagnostics::getXmlRoot()
{
    return doc_.FirstChildElement("root");
}


int AvDiagnostics::checkXml()
{
    // проверка xml
    return 1;
}

std::pair<std::string, int> AvDiagnostics::getOpcAddress() {

    std::string opc_address = "192.168.6.72";
    int opc_port = 62544;

    if (auto* root = doc_.FirstChildElement("root")) {
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
    return {opc_address, opc_port};
}

// -------- CPU ------------

bool AvDiagnostics::loadCpuConfig() {

    auto* root = getXmlRoot();
    auto* av_diag = root->FirstChildElement("AvDiagnostics");

    auto* cpus = av_diag->FirstChildElement("CPUs");

    for (auto* cpu = cpus->FirstChildElement("CPU"); cpu; cpu = cpu->NextSiblingElement("CPU"))
    {
        const char* number = cpu->Attribute("number");
        const char* node = cpu->Attribute("node");
        const char* type = cpu->Attribute("type");

        cpu_nodes_[std::string(number)] = NodeInfo{std::string(node), std::string(type)};
    }

    return 1;
}


std::vector<AvDiagnostics::CpuTimes> AvDiagnostics::readCpuTimes() {

    std::ifstream file("/proc/stat");
    std::string line;
    std::vector<CpuTimes> times;

    while (std::getline(file, line)) {

        if (line.compare(0, 3, "cpu") != 0 || line.compare(0, 4, "cpu ") == 0)
            continue;

        std::istringstream ss(line);
        std::string cpu;
        CpuTimes t = {};
        ss >> cpu >> t.user >> t.nice >> t.system >> t.idle >> t.iowait >> t.irq >> t.softirq >> t.steal;
        times.push_back(t);
    }

    return times;
}


std::vector<double> AvDiagnostics::getCpuUsage() {

    auto t1 = readCpuTimes();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    auto t2 = readCpuTimes();
    std::vector<double> usages;

    for (size_t i = 0; i < t1.size(); ++i) {

        unsigned long long idle = t2[i].idle - t1[i].idle;
        unsigned long long total = t2[i].total() - t1[i].total();
        double usage = 0.0;

        if (total != 0)
            usage = 100.0 * (1.0 - (double)idle / total);

        usages.push_back(usage);
    }

    return usages;
}

std::map<std::string, int> AvDiagnostics::collectCpuUsage()
{

    auto cpu_usages_vec = getCpuUsage();
    std::map<std::string, int> current_usages;

    for (size_t i = 0; i < cpu_usages_vec.size(); ++i) {
        std::string cpu_index = std::to_string(i);
        if (cpu_nodes_.find(cpu_index) != cpu_nodes_.end()) {
            current_usages[cpu_index] = static_cast<int>(cpu_usages_vec[i]);
        }
    }

    return current_usages;
}


bool AvDiagnostics::checkForCpuChanges(const std::map<std::string, int>& current_usages)
{
    const int threshold = 1;

    if (last_sent_cpu_usages_.size() != current_usages.size()) {
        return true;
    }

    for (const auto& [cpu, usage] : current_usages) {
        auto it = last_sent_cpu_usages_.find(cpu);
        if (it == last_sent_cpu_usages_.end() || abs(it->second - usage) > threshold) {
            return true;
        }
    }
    return false;
}



bool AvDiagnostics::sendCpuUsage(OpcUaClient& opcClient, const std::map<std::string, int>& current_usages)
{
    std::vector<OpcUaClient::WriteConfig> writeConfigs;

    for (const auto& [cpu_name, usage] : current_usages) {
        auto node_it = cpu_nodes_.find(cpu_name);
        if (node_it != cpu_nodes_.end()) {
            OpcUaClient::WriteConfig config;

            config.allowed = true;
            config.node_id = node_it->second.node;
            config.type = node_it->second.type;

            if (config.type == "STRING") {
                config.value.s = std::to_string(usage);
            } else if (config.type == "INT") {
                config.value.i = usage;
            } else {
                config.value.f = static_cast<float>(usage);
            }

            writeConfigs.push_back(config);
        }
    }

    if (!writeConfigs.empty()) {
           opcClient.Write(writeConfigs);
           last_sent_cpu_usages_ = current_usages;
           return 1;
       }
    return 0;
}

// --------------- ДИСКИ ------------------------

bool AvDiagnostics::loadDiskConfigs() {

    auto* root = getXmlRoot();

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

        disk_configs_[name] = cfg;
    }

    return !disk_configs_.empty();
}

std::map<std::string, AvDiagnostics::DiskUsageData> AvDiagnostics::collectDiskUsage() {

    std::map<std::string, DiskUsageData> current_data;

        loadDiskConfigs();

    for (const auto& [name, _] : disk_configs_) {
        struct statvfs stat;
        if (statvfs(name.c_str(), &stat) != 0) {
            continue;
        }

        DiskUsageData data;
        data.total = stat.f_blocks * stat.f_frsize;
        data.available = stat.f_bavail * stat.f_frsize;
        data.used = data.total - data.available;
        data.use_percent = (data.total == 0) ? 0 : static_cast<int>((data.used * 100) / data.total);


        struct statfs fs_info;
                if (statfs(name.c_str(), &fs_info) == 0) {
                    data.file_system = getFileSystemType(fs_info.f_type);
                } else {
                    data.file_system = "unknown";
                }

                data.mount_point = name;

        current_data[name] = data;
    }

    return current_data;
}


bool AvDiagnostics::HasDiskChanged(const DiskUsageData& old_data, const DiskUsageData& new_data) {
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

void AvDiagnostics::sendFullDiskMetrics(
    OpcUaClient& opcClient,
    const std::vector<std::pair<std::string, DiskUsageData>>& changed_disks,
    const std::map<std::string, DiskConfig>& disk_configs)
{
    std::vector<OpcUaClient::WriteConfig> writeConfigs;


    auto sendDiskMetric = [&](const std::string& node_id, const std::string& type, float value) {
        OpcUaClient::WriteConfig cfg;
        cfg.allowed = true;
        cfg.node_id = node_id;
        cfg.type = type;

        if (type == "STRING") {
            cfg.value.s = std::to_string(value);
        } else if (type == "INT") {
            cfg.value.i = static_cast<int>(value);
        } else {
            cfg.value.f = static_cast<double>(value);
        }

        writeConfigs.push_back(std::move(cfg));
    };

    for (const auto& [mount_point, data] : changed_disks) {
        auto it = disk_configs.find(mount_point);
        if (it == disk_configs.end()) {
            continue;
        }

        const auto& cfg = it->second;

        if (!cfg.node_total.empty()) {
            sendDiskMetric(cfg.node_total, cfg.type_total, convertUnits(data.total, cfg.units_total, cfg.precision_total));
            if (!cfg.node_units_total.empty()) {
                writeConfigs.push_back({true, cfg.node_units_total, cfg.type_units_total, .value = {.s = cfg.units_total}});
            }
        }

        if (!cfg.node_used.empty()) {
            sendDiskMetric(cfg.node_used, cfg.type_used, convertUnits(data.used, cfg.units_used, cfg.precision_used));
            if (!cfg.node_units_used.empty()) {
                writeConfigs.push_back({true, cfg.node_units_used, cfg.type_units_used, .value = {.s = cfg.units_used}});
            }
        }

        if (!cfg.node_available.empty()) {
            sendDiskMetric(cfg.node_available, cfg.type_available, convertUnits(data.available, cfg.units_available, cfg.precision_available));
            if (!cfg.node_units_available.empty()) {
                writeConfigs.push_back({ true, cfg.node_units_available, cfg.type_units_available, .value = {.s = cfg.units_available}
                });
            }
        }

        if (!cfg.node_file_system.empty() && !data.file_system.empty()) {
            writeConfigs.push_back({ true, cfg.node_file_system, cfg.type_file_system, .value = {.s = data.file_system}
            });
        }

        if (!cfg.node_mount_point.empty()) {
            writeConfigs.push_back({true, cfg.node_mount_point, cfg.type_mount_point, .value = {.s = data.mount_point}
            });
        }

        if (!cfg.node_percent.empty()) {
            sendDiskMetric(cfg.node_percent, cfg.type_percent, data.use_percent);
        }
    }

    if (!writeConfigs.empty()) {
        opcClient.Write(writeConfigs);
    }
}
