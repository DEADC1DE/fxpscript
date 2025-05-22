#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <fstream>      
#include <sstream>      
#include <chrono>      
#include <iomanip>      
#include <cstdlib>      

// --- KONFIGURATION ---
const std::string RULES_FILE_PATH = "/etc/fxp.rules"; 
const std::string GLLOG_PATH = "/ftp-data/logs/glftpd.log"; 

const bool ENABLE_GLLOG_MESSAGES = true; 
// --- ENDE KONFIGURATION ---

void load_rules_from_file(std::unordered_set<std::string>& specific_rules,
                          std::unordered_set<std::string>& both_rules) {
    std::ifstream rules_file(RULES_FILE_PATH);
    std::string line;

    if (!rules_file.is_open()) {
        if (ENABLE_GLLOG_MESSAGES) { 
            std::ofstream log_file(GLLOG_PATH, std::ios_base::app);
            if (log_file.is_open()) {
                auto now = std::chrono::system_clock::now();
                auto now_c = std::chrono::system_clock::to_time_t(now);
                log_file << std::put_time(std::localtime(&now_c), "%a %b %d %T %Y")
                         << " FXP_CHECK_ERROR: Could not open rules file: " << RULES_FILE_PATH << std::endl;
            }
        }
        return;
    }

    while (std::getline(rules_file, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty() || line[0] == '#') {
            continue;
        }

        size_t tilde_pos = line.find('~');
        if (tilde_pos != std::string::npos) {
            std::string ip = line.substr(0, tilde_pos);
            std::string dir = line.substr(tilde_pos + 1);

            if (dir == "both") {
                both_rules.insert(ip);
            } else if (dir == "from" || dir == "to") {
                specific_rules.insert(line);
            }
        }
    }
    rules_file.close();
}

void write_log_entry(const std::string& message) {
    if (!ENABLE_GLLOG_MESSAGES) { 
        return;
    }

    std::ofstream log_file(GLLOG_PATH, std::ios_base::app);
    if (log_file.is_open()) {
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);
        log_file << std::put_time(std::localtime(&now_c), "%a %b %d %T %Y") << " " << message << std::endl;
        log_file.close();
    } else {
    }
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        return 1; // Ugly
    }

    const char* direction_fxp_cstr = argv[1];
    const char* ipv4_cstr = argv[2];
    const char* file_name_cstr = argv[3];
    const char* file_path_cstr = argv[4];

    std::string full_path_to_check = std::string(file_path_cstr) + "/" + std::string(file_name_cstr);
    std::ifstream file_to_check(full_path_to_check.c_str());
    if (!file_to_check.good()) {
        file_to_check.close();
        return 0;
    }
    file_to_check.close();

    std::unordered_set<std::string> specific_rules;
    std::unordered_set<std::string> both_rules;

    load_rules_from_file(specific_rules, both_rules);

    if (specific_rules.empty() && both_rules.empty()) {
        return 0;
    }

    std::string target_ip(ipv4_cstr);
    std::string target_direction(direction_fxp_cstr);
    std::string target_specific = target_ip + "~" + target_direction;

    bool denied = false;
    std::string matched_rule_info_for_log;

    if (specific_rules.count(target_specific)) {
        denied = true;
        matched_rule_info_for_log = target_specific;
    } else if (both_rules.count(target_ip)) {
        denied = true;
        matched_rule_info_for_log = target_ip + "~both (matched " + target_ip + ")";
    }

    if (denied) {
        if (ENABLE_GLLOG_MESSAGES) {
            std::stringstream log_message;
            log_message << "BAD_FXP_DENIED: ";

            const char* user_env = std::getenv("USER");
            if (user_env != nullptr) {
                log_message << "User '" << user_env << "' ";
            } else {
                log_message << "User 'UNKNOWN' ";
            }

            log_message << "Action '" << target_direction << "' "
                        << "IP '" << target_ip << "' "
                        << "File '" << file_path_cstr << "/" << file_name_cstr << "' "
                        << "Rule '" << matched_rule_info_for_log << "'";

            write_log_entry(log_message.str());
        }
				// std::cout << "Sorry not from this source." << std::endl; // Wichtig: std::endl flusht den Buffer
        return 2; // Bad
    }

    return 0; // Good
}