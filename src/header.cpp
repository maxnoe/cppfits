#include "fits/header.h"
#include <iostream>
#include <regex>

const std::regex integer_regex{R"([+-]?\d+)"};
const std::regex floating_regex{R"([+-]?(\d+)?[.]?(\d+)?([eEdD][+-]?\d+)?)"};

namespace fits {


void unescape_single_quotes(std::string& s) {
    size_t pos = s.find("''");
    while (pos != std::string::npos) {
        s.replace(pos, 2, "'");
        pos = s.find("''", pos);
    }
}


HeaderEntry::value_t parse_value(std::string_view s) {
    s = strip(s);

    if (s.at(0) == '\'') {
        s.remove_prefix(1);
        if (s.back() != '\'') {
            throw std::runtime_error("Invalid string in header '" + std::string{s} + "'");
        }
        s.remove_suffix(1);
        std::string str{s};
        unescape_single_quotes(str);
        return str;
    }

    s = strip(s);
    if (s == "T") {
        return true;
    }
    if (s == "F") {
        return false;
    }

    std::string str{s};
    if (std::regex_match(str, integer_regex)) {
        return std::stoll(str);
    }
    if (std::regex_match(str, floating_regex)) {
        return std::stod(str);
    }
    throw std::runtime_error(std::string(s));
}

HeaderEntry HeaderEntry::parse(std::string_view line) {
    std::string key{right_strip(line.substr(0, 8))};
    std::string comment;
    HeaderEntry::value_t value;

    if (key == "COMMENT" || key == "HISTORY") {
        comment = std::string(right_strip(line.substr(8)));
    } else {
        size_t comment_start = line.find_first_of('/');
        if (comment_start != std::string_view::npos) {
            comment = strip(line.substr(comment_start + 1));
            line.remove_suffix(line.size() - comment_start);
        }

        if (line.substr(8, 2) == "= ") {
            try {
                value = parse_value(line.substr(10));
            } catch (...) {
                throw std::runtime_error("Error parsing header line:" + std::string{line});
            }
        }
    }

    return HeaderEntry(key, value, comment);
};


}
