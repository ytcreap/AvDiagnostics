#ifndef AVDIAGNOSTICS_H
#define AVDIAGNOSTICS_H

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

class AvDiagnostics
{
public:

    struct RaidState {
        std::string status;
        bool state;
        double rebuild_percent;
    };

    struct RaidNodes{
            std::string node_status;
            std::string type_status;
            std::string node_state;
            std::string type_state;
        };


    struct RamState {
        unsigned long long total;
        unsigned long long used;
        unsigned long long free;
        std::time_t last_update;
    };

    struct RamNodes {
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

    struct CpuTimes {
        unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
        unsigned long long total() const {
            return user + nice + system + idle + iowait + irq + softirq + steal;
        }
    };

    struct NodeInfo {
        std::string node;
        std::string type;
    };

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

    struct ProcessState {
        std::string status;
        bool is_group_result = false;
        std::string group_node;
        std::string value_type;
    };

    struct IpState {
        bool connected;
        std::time_t last_change;
        bool is_group_result;
        std::string group_node;
        std::string value_type;
    };


    AvDiagnostics()
    {
        // конструктор

    }

    ~AvDiagnostics()
    {
        // Деструктор
        //
        //  Отключение от opc
        //  Конец всех циклов
        //  Очистка
        //
    }

    // XML и OPC -----------

    int loadXml();
    tinyxml2::XMLElement* getXmlRoot();
    int checkXml();
    std::pair<std::string, int> getOpcAddress();

    // CPU -----------------

    bool loadCpuConfig();

    std::vector<CpuTimes> readCpuTimes();

    std::vector<double> getCpuUsage();

    std::map<std::string, int> collectCpuUsage();

    bool checkForCpuChanges(const std::map<std::string, int>&);

    bool sendCpuUsage(OpcUaClient&, const std::map<std::string, int>&);

    // Диски ---------------

    bool loadDiskConfigs();

    std::string getFileSystemType(long f_type) {
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

    std::map<std::string, DiskUsageData> collectDiskUsage();
    bool hasDiskChanged(const DiskUsageData&, const DiskUsageData&);
    std::vector<std::pair<std::string, DiskUsageData>> findChangedDisks( const std::map<std::string, DiskUsageData>&);
    double convertUnits(uint64_t, const std::string&, int);


    std::map<std::string, NodeInfo> cpu_nodes_;
    std::map<std::string, int> last_sent_cpu_usages_;
    std::map<std::string, DiskConfig> disk_configs_;
    std::map<std::string, DiskUsageData> last_sent_disk_usage_;
private:
    tinyxml2::XMLDocument doc_;

};

#endif // AVDIAGNOSTICS_H
