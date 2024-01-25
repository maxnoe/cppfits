#include <iostream>
#include <regex>
#include "fits/header.h"

const std::regex int_regex{R"([+-]? *\d+)"};
const std::regex double_regex{R"([+-]? *(\d+)?[.]?(\d+)? *([eEdD] *[+-]? *\d+)?)"};
const std::regex string_regex{R"('((?:[^']|'')*)'(?: */ *(.*)) *?)"};

namespace fits {


void unescape_single_quotes(std::string& s) {
    size_t pos = s.find("''");
    while (pos != std::string::npos) {
        s.replace(pos, 2, "'");
        pos = s.find("''", pos);
    }
}

void replace_fits_exponent(std::string& s) {
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
    if (std::regex_match(str, int_regex)) {
        return static_cast<int64_t>(std::stoll(str));
    }
    if (std::regex_match(str, double_regex)) {
        replace_fits_exponent(str);
        return std::stod(str);
    }
    throw std::runtime_error(std::string(s));
}

HeaderEntry HeaderEntry::parse(std::string_view line) {
    std::string key{right_strip(line.substr(0, 8))};
    std::string comment;
    HeaderEntry::value_t value;

    // Blank
    if (key == "") {
        return HeaderEntry();
    }

    // Comment and History fields have the comment as rest of the line
    if (key == "COMMENT" || key == "HISTORY") {
        comment = std::string(right_strip(line.substr(8)));
        return HeaderEntry(key, HeaderEntry::no_value{}, comment);
    }

    // The value indicator
    if (line.substr(8, 2) == "= ") {
        std::string_view value_part = line.substr(10);

        // First check for string, since the string regex catches
        // the case where / might appear inside the string
        std::string value_str{value_part};
        std::smatch match;
        if (std::regex_match(value_str, match, string_regex)) {
            value = match[1];
            std::string m2 = match[2];
            comment = std::string(right_strip(m2));

            return HeaderEntry(key, value, comment);
        }

        size_t comment_start = value_part.find_first_of('/');
        if (comment_start != std::string_view::npos) {
            comment = strip(value_part.substr(comment_start + 1));
            value_part.remove_suffix(value_part.size() - comment_start);
        }

        try {
            value = parse_value(value_part);
        } catch (std::exception& e) {
            throw std::runtime_error("Error parsing header line:" + std::string{line} + e.what());
        }

    }

    return HeaderEntry(key, value, comment);
}

}
