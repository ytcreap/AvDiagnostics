#include "avdiagnostics.h"

int AvDiagnostics::loadXml() {
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
    // проверка xml TODO
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

    for (auto* cpu = cpus->FirstChildElement("CPU"); cpu; cpu = cpu->NextSiblingElement("CPU")) {
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

bool AvDiagnostics::collectCpuUsage() {
    auto cpu_usages_vec = getCpuUsage();
    std::map<std::string, int> current_usages;

    for (size_t i = 0; i < cpu_usages_vec.size(); ++i) {
        std::string cpu_index = std::to_string(i);
        if (cpu_nodes_.find(cpu_index) != cpu_nodes_.end()) {
            current_usages[cpu_index] = static_cast<int>(cpu_usages_vec[i]);
        }
    }
    current_cpu_usages_ = current_usages;
    return 1;
}


bool AvDiagnostics::checkForCpuChanges() {
    const int threshold = 1;

    if (last_sent_cpu_usages_.size() != current_cpu_usages_.size()) {
        return true;
    }

    for (const auto& [cpu, usage] : current_cpu_usages_) {
        auto it = last_sent_cpu_usages_.find(cpu);
        if (it == last_sent_cpu_usages_.end() || abs(it->second - usage) > threshold) {
            return true;
        }
    }
    return false;
}



bool AvDiagnostics::sendCpuUsage(OpcUaClient& opcClient) {
    std::vector<OpcUaClient::WriteConfig> writeConfigs;

    for (const auto& [cpu_name, usage] : current_cpu_usages_) {
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
           last_sent_cpu_usages_ = current_cpu_usages_;
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

        if (disk->Attribute("node_total")) cfg.node_total = disk->Attribute("node_total");
        if (disk->Attribute("type_total")) cfg.type_total = disk->Attribute("type_total");
        if (disk->Attribute("node_units_total")) cfg.node_units_total = disk->Attribute("node_units_total");
        if (disk->Attribute("units_total")) cfg.units_total = disk->Attribute("units_total");
        if (disk->QueryIntAttribute("precision_total", &cfg.precision_total) != tinyxml2::XML_SUCCESS) {
            cfg.precision_total = 0;
        }

        if (disk->Attribute("node_used")) cfg.node_used = disk->Attribute("node_used");
        if (disk->Attribute("type_used")) cfg.type_used = disk->Attribute("type_used");
        if (disk->Attribute("node_units_used")) cfg.node_units_used = disk->Attribute("node_units_used");
        if (disk->Attribute("units_used")) cfg.units_used = disk->Attribute("units_used");
        if (disk->QueryIntAttribute("precision_used", &cfg.precision_used) != tinyxml2::XML_SUCCESS) {
            cfg.precision_used = 0;
        }

        if (disk->Attribute("node_available")) cfg.node_available = disk->Attribute("node_available");
        if (disk->Attribute("type_available")) cfg.type_available = disk->Attribute("type_available");
        if (disk->Attribute("node_units_available")) cfg.node_units_available = disk->Attribute("node_units_available");
        if (disk->Attribute("units_available")) cfg.units_available = disk->Attribute("units_available");
        if (disk->QueryIntAttribute("precision_available", &cfg.precision_available) != tinyxml2::XML_SUCCESS) {
            cfg.precision_available = 0;
        }

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

void AvDiagnostics::collectDiskUsage() {
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
        current_disk_usage_[name] = data;
    }
}


bool AvDiagnostics::hasDiskChanged(const DiskUsageData& old_data, const DiskUsageData& new_data) {
    return (old_data.use_percent != new_data.use_percent ||
            old_data.used != new_data.used ||
            old_data.total != new_data.total);
}

std::vector<std::pair<std::string, AvDiagnostics::DiskUsageData>> AvDiagnostics::findChangedDisks() {
    std::vector<std::pair<std::string, DiskUsageData>> changed;
    for (const auto& [mp, data] : current_disk_usage_) {
        auto it = last_sent_disk_usage_.find(mp);
        if (it == last_sent_disk_usage_.end()) {
            // Новый диск
            changed.emplace_back(mp, data);
            last_sent_disk_usage_[mp] = data;
        } else if (hasDiskChanged(it->second, data)) {
            // Данные изменились
            changed.emplace_back(mp, data);
            last_sent_disk_usage_[mp] = data;
        }
    }
    return changed;
}

void AvDiagnostics::sendFullDiskMetrics(OpcUaClient& opcClient, const std::vector<std::pair<std::string, DiskUsageData>>& changed_disks) {

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
        auto it = disk_configs_.find(mount_point);
        if (it == disk_configs_.end()) {
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

// RAM -------------------
void AvDiagnostics::loadRamConfig() {
    auto* root = getXmlRoot();
    auto* av_diag = root->FirstChildElement("AvDiagnostics");
    auto* ram = av_diag->FirstChildElement("RAM");

    if (auto* total = ram->FirstChildElement("TotalRAM")) {
        ram_config_.node_total = total->Attribute("node");
        ram_config_.type_total = total->Attribute("type");
        total->QueryIntAttribute("precision", &ram_config_.precision_total);
    }

    if (auto* used = ram->FirstChildElement("UsedRAM")) {
        ram_config_.node_used = used->Attribute("node") ? used->Attribute("node") : "";
        ram_config_.type_used = used->Attribute("type") ? used->Attribute("type") : "REAL";
        used->QueryIntAttribute("precision", &ram_config_.precision_used);
    }

    if (auto* free = ram->FirstChildElement("FreeRAM")) {
        ram_config_.node_free = free->Attribute("node") ? free->Attribute("node") : "";
        ram_config_.type_free = free->Attribute("type") ? free->Attribute("type") : "REAL";
        free->QueryIntAttribute("precision", &ram_config_.precision_free);
    }
}

AvDiagnostics::RamState AvDiagnostics::collectRamUsage() {
    RamState state;
    std::array<char, 128> buffer;
    std::string line;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("free -k | grep Mem:", "r"), pclose);
    if (!pipe) return state;
    if (fgets(buffer.data(), buffer.size(), pipe.get())) {
        line = buffer.data();
    }
    state.last_update = std::time(nullptr);
    return state;
}

bool AvDiagnostics::isRamChanged(const RamState& current) {
    const unsigned long long threshold = 1024; // 1MB
    return
        std::abs(static_cast<long>(current.total - last_sent_ram_.total)) > threshold ||
        std::abs(static_cast<long>(current.used - last_sent_ram_.used)) > threshold ||
        std::abs(static_cast<long>(current.free - last_sent_ram_.free)) > threshold;
}


void AvDiagnostics::sendRamMetrics(OpcUaClient& opcClient, const RamState& data) {
    std::vector<OpcUaClient::WriteConfig> writeConfigs;
    // Total RAM
    if (!ram_config_.node_total.empty()) {
        OpcUaClient::WriteConfig cfg;
        cfg.allowed = true;
        cfg.node_id = ram_config_.node_total;
        cfg.type = ram_config_.type_total;

        float value = convertUnits(data.total, "КБ", ram_config_.precision_total);
        if (ram_config_.type_total == "INT") {
            cfg.value.i = static_cast<int>(value);
        } else {
            cfg.value.f = static_cast<float>(value);
        }
        writeConfigs.push_back(cfg);
    }

    // Used RAM
    if (!ram_config_.node_used.empty()) {
        OpcUaClient::WriteConfig cfg;
        cfg.allowed = true;
        cfg.node_id = ram_config_.node_used;
        cfg.type = ram_config_.type_used;

        float value = convertUnits(data.used, "КБ", ram_config_.precision_used);
        if (ram_config_.type_used == "INT") {
            cfg.value.i = static_cast<int>(value);
        } else {
            cfg.value.f = static_cast<float>(value);
        }
        writeConfigs.push_back(cfg);
    }

    // Free RAM
    if (!ram_config_.node_free.empty()) {
        OpcUaClient::WriteConfig cfg;
        cfg.allowed = true;
        cfg.node_id = ram_config_.node_free;
        cfg.type = ram_config_.type_free;

        float value = convertUnits(data.free, "КБ", ram_config_.precision_free);
        if (ram_config_.type_free == "INT") {
            cfg.value.i = static_cast<int>(value);
        } else {
            cfg.value.f = static_cast<float>(value);
        }
        writeConfigs.push_back(cfg);
    }

    //std::cerr << "\n node: " << writeConfigs[0].node_id << "\n type" << writeConfigs[0].type << "\n value" << writeConfigs[0].value.f;
    if (!writeConfigs.empty()) {
        opcClient.Write(writeConfigs);
        last_sent_ram_ = data;
    }
}

// Processes --------------------

std::map<std::string, std::vector<int>> getRunningProcesses() {
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

void AvDiagnostics::collectProcessStates() {
    std::map<std::string, ProcessState> current_states;
    auto* root = getXmlRoot();
    auto* av_diag = root->FirstChildElement("AvDiagnostics");
    auto running_processes = getRunningProcesses();

    for (auto* processes = av_diag->FirstChildElement("Processes"); processes; processes = processes->NextSiblingElement("Processes")) {
        const char* group_node = processes->Attribute("node");
        const char* logic = processes->Attribute("logic");
        const char* type = processes->Attribute("type");
        if (!group_node || !type || (logic && std::string(logic) != "AND"))
            continue;
        std::vector<std::string> members;
        for (auto* proc = processes->FirstChildElement("Process"); proc; proc = proc->NextSiblingElement("Process")) {
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
         ProcessState group_state;
         group_state.status = (all_exist && all_running) ? "RUNNING" : "NOT_RUNNING";
         group_state.is_group_result = true;
         group_state.group_node = group_node;
         group_state.value_type = type;
         current_states[group_node] = group_state;
         }
     }
    current_process_states_ = current_states;
}

bool AvDiagnostics::detectProcessChanges() {
    for (const auto& [name, state] : current_process_states_) {
        if (!state.is_group_result) continue;
        auto it = last_sent_process_states_.find(name);
        if (it == last_sent_process_states_.end() || it->second.status != state.status) {
            return true;
        }
    }
    return false;
}

void AvDiagnostics::sendProcessMetrics(OpcUaClient &opcClient) {
    std::vector<OpcUaClient::WriteConfig> writes;

    for (const auto& [name, state] : current_process_states_) {
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
        last_sent_process_states_[name] = state;
    }
    if (!writes.empty() && opcClient.CheckConnection()) {
        opcClient.Write(writes);
    }
}

// RAID ------------------

bool AvDiagnostics::loadRaidConfig() {
    auto* root = getXmlRoot();
    auto* av_diag = root->FirstChildElement("AvDiagnostics");
    auto* raids = av_diag->FirstChildElement("RAIDs");
    for (auto* raid = raids->FirstChildElement("RAID"); raid; raid = raid->NextSiblingElement("RAID")) {
        const char* name = raid->Attribute("name");
        if (!name) continue;
        raid_configs_[name] = {
            .node_status = raid->Attribute("node_status"),
            .type_status = raid->Attribute("type_status"),
            .node_state = raid->Attribute("node_state"),
            .type_state = raid->Attribute("type_state"),
        };
    }
    return !raid_configs_.empty();
}


void AvDiagnostics::collectRaidStates() {
    for (const auto& [raid_name, config] : raid_configs_) {
        RaidState raid_status;
        raid_status.is_ok = false;
        raid_status.status = "";
        if (raid_name == "Adaptec") {
            system("./arcconf getconfig 1 ld > result.txt");
            std::ifstream file("result.txt");
            std::string line;
            while (std::getline(file, line)) {
                if (line.find("Status") != std::string::npos) {
                    if (line.find("Optimal") != std::string::npos) {
                        raid_status.is_ok = true;
                    }
                    else if (line.find("Degraded") != std::string::npos) {
                        raid_status.is_ok = false;
                    }
                    raid_status.status = line.substr(line.find(": ") + 2);
                    break;
                }
            }
        }
        else if (raid_name == "LSI") {
            system("megacli -LDInfo -LALL -aALL | grep State > result.txt");
            std::ifstream file("result.txt");
            std::string line;

            while (std::getline(file, line)) {
                if (line.find("State") != std::string::npos) {
                    if (line.find("Optimal") != std::string::npos) {
                        raid_status.is_ok = true;
                        raid_status.status = "Optimal";
                        break;
                    }
                    else if (line.find("Degraded") != std::string::npos) {
                        raid_status.is_ok = false;
                        raid_status.status = "Degraded";
                    }
                }
            }

            if (raid_status.is_ok == false) {
                for (int dev = 0; dev <= 1; ++dev) {
                    std::string cmd = "megacli -PDRbld -showprog -PhysDrv[252:" + std::to_string(dev) + "] -aALL > result.txt";
                    system(cmd.c_str());
                    std::ifstream file1("result.txt");
                    while (std::getline(file1, line)) {
                        if (line.find("Rebuild Progress on Device") != std::string::npos) {
                            raid_status.status = line;
                            break;
                        }
                    }
                }
            }
        }
        else {
            raid_status.is_ok = false;
            raid_status.status = "unknown";
        }
        current_raids_[raid_name] = raid_status;
    }
}

std::map<std::string, AvDiagnostics::RaidState> AvDiagnostics::detectRaidChanges() {
    static bool first_run = true;
    std::map<std::string, RaidState> changed;

    if (first_run) {
        last_sent_raids_ = current_raids_;
        first_run = false;
        return current_raids_;
    }

    for (const auto& [name, current_state] : current_raids_) {
        auto last_it = last_sent_raids_.find(name);
        // Новый RAID
        if (last_it == last_sent_raids_.end()) {
            changed[name] = current_state;
            continue;
        }
        // Сравнение состояний
        const auto& last_state = last_it->second;
        bool has_changes = false;
        if (last_state.status != current_state.status) {
            has_changes = true;
        }
        if (last_state.is_ok != current_state.is_ok) {
            has_changes = true;
        }
        if (has_changes) {
            changed[name] = current_state;
        }
    }
    // Поиск удаленных RAID
    for (const auto& [name, _] : last_sent_raids_) {
        if (current_raids_.find(name) == current_raids_.end()) {
            RaidState removed_state;
            removed_state.status = "removed";
            removed_state.is_ok = false;
            changed[name] = removed_state;
        }
    }
    return changed;
}


void AvDiagnostics::sendRaidMetrics(OpcUaClient& opcClient, const std::map<std::string, RaidState>& changed){
    if (changed.empty()) return;
    std::vector<OpcUaClient::WriteConfig> writeConfigs;
    for (const auto& [name, state] : changed) {
        auto config_it = raid_configs_.find(name);
        if (config_it == raid_configs_.end()) continue;

        // Статус RAID
        OpcUaClient::WriteConfig status_cfg;
        status_cfg.allowed = true;
        status_cfg.node_id = config_it->second.node_status;
        status_cfg.type = config_it->second.type_status;
        status_cfg.value.s = state.status;
        writeConfigs.push_back(status_cfg);

        // Исправность
        OpcUaClient::WriteConfig health_cfg;
        health_cfg.allowed = true;
        health_cfg.node_id = config_it->second.node_state;
        health_cfg.type = config_it->second.type_state;
        if (health_cfg.type == "STRING") {
            health_cfg.value.s = std::to_string(state.is_ok);
        } else if (health_cfg.type == "BOOL") {
            health_cfg.value.b = state.is_ok;
        } else {
            health_cfg.value.f = static_cast<float>(state.is_ok);
        }
        writeConfigs.push_back(health_cfg);
    }

    if (!writeConfigs.empty()) {
        opcClient.Write(writeConfigs);
        for (const auto& [name, state] : changed) {
            if (state.status == "removed") {
                last_sent_raids_.erase(name);
            } else {
                last_sent_raids_[name] = state;
            }
        }
    }
}

// IP --------------------

bool pingIp(const std::string& ip) {
    std::string cmd = "ping -c 1 -W 1 " + ip + " >/dev/null 2>&1";
    int ret = system(cmd.c_str());
    return (ret == 0);
}

std::map<std::string, bool> AvDiagnostics::loadIps() {
    std::map<std::string, bool> ip_status;
    std::vector<std::pair<std::string, std::future<bool>>> async_checks;

        auto* root = getXmlRoot();
        auto* av_diag = root->FirstChildElement("AvDiagnostics");
        for (auto* ips_xml = av_diag->FirstChildElement("IPs"); ips_xml; ips_xml = ips_xml->NextSiblingElement("IPs")) {
            for (auto* ip = ips_xml->FirstChildElement("IP"); ip; ip = ip->NextSiblingElement("IP")) {
                if (const char* addr = ip->Attribute("address")) {
                    ip_status[addr] = false;
                }
            }
        }

    for (auto& [ip, status] : ip_status) {

        int ping_result = system(("timeout 0.01 ping -c 1 " + ip + " >/dev/null 2>&1").c_str());
        if (ping_result == 0) {
            status = true;
        } else {
            std::string ip_async = ip;
            async_checks.emplace_back(ip_async, std::async(std::launch::async, [ip_async]() {
            return pingIp(ip_async);
        }));
        }
    }
    for (auto& [ip, future] : async_checks) {
        if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            ip_status[ip] = future.get();
        }
    }
    return ip_status;
}


std::map<std::string, AvDiagnostics::IpState> AvDiagnostics::collectIpStates() {
    std::map<std::string, IpState> current_states;
    std::map<std::string, std::pair<std::vector<std::string>, std::string>> ip_groups;

    auto* root = getXmlRoot();
    auto* av_diag = root->FirstChildElement("AvDiagnostics");
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

    auto ip_status = loadIps();

    // Формирование состояний отдельных IP
    for (const auto& [ip, connected] : ip_status) {
        IpState state;
        state.connected = connected;
        state.last_change = std::time(nullptr);
        state.is_group = false;
        current_states[ip] = state;
    }

    // Проверка групп IP и формирование групповых состояний в зависимости от логики
    for (const auto& [group_node, group_data] : ip_groups) {
        const auto& [ips, logic] = group_data;
        bool group_result = (logic == "AND");

        for (const auto& ip : ips) {
            auto it = ip_status.find(ip);
            bool connected = (it != ip_status.end() && it->second);

            if (logic == "AND") {
                group_result &= connected;
            } else { // OR
                group_result |= connected;
            }
        }
        IpState group_state;
        group_state.connected = group_result;
        group_state.last_change = std::time(nullptr);
        group_state.is_group = true;
        group_state.group_node = group_node;
        group_state.type = "BOOL";
        current_states[group_node] = group_state;
    }
    return current_states;
}


std::map<std::string, AvDiagnostics::IpState> AvDiagnostics::detectIpChanges() {
    std::map<std::string, IpState> changes;
    for (const auto& [node, state] : current_ips_) {
        if (!last_sent_ips_.count(node) || last_sent_ips_[node].connected != state.connected) {
            changes[node] = state;
        }
    }
    last_sent_ips_ = current_ips_;
    return changes;
}

void AvDiagnostics::sendIpMetrics(OpcUaClient& opcClient, std::map<std::string, IpState>& changes) {
    std::vector<OpcUaClient::WriteConfig> writeConfigs;
    for (const auto& [node, state] : changes) {
        if (!state.is_group) continue;
        OpcUaClient::WriteConfig cfg;
        cfg.node_id = node;
        cfg.type = state.type;
        if (state.type == "BOOL") cfg.value.b = state.connected ? 1 : 0;
        else if (state.type == "INT") cfg.value.i = state.connected ? 1 : 0;
        else if (state.type == "STRING") cfg.value.s = state.connected ? "true" : "false";
        cfg.type = "BOOL";
        writeConfigs.push_back(cfg);
    }
    if (!writeConfigs.empty() && opcClient.CheckConnection()) {
        opcClient.Write(writeConfigs);
    }
}

// Guardant ----------------

