#include <iostream>
#include <regex>
#include "fits/header.h"

const std::regex integer_regex{R"([+-]?\d+)"};
const std::regex replace_fits_exponent{R"([+-]?(\d+)?[.]?(\d+)?([eEdD][+-]?\d+)?)"};

namespace fits {


void unescape_single_quotes(std::string& s) {
    size_t pos = s.find("''");
    while (pos != std::string::npos) {
        s.replace(pos, 2, "'");
        pos = s.find("''", pos);
    }
}

void replace_fits_expent(std::string& s) {
    for (auto& exp: {"d", "D"}) {
        size_t pos = s.find(exp);
        if (pos != std::string::npos) {
            s.replace(pos, 1, "e");
        }
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
        return static_cast<int64_t>(std::stoll(str));
    }
    if (std::regex_match(str, replace_fits_exponent)) {
        replace_fits_expent(str);
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
            } catch (std::exception& e) {
                throw std::runtime_error("Error parsing header line:" + std::string{line} + e.what());
            }
        }
    }

    return HeaderEntry(key, value, comment);
};


}
