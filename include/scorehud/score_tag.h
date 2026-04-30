#pragma once

#include <string>

namespace scorehud {

class ScoreTag {
public:
    ScoreTag(std::string name, std::string content = "");

    [[nodiscard]] const std::string &name() const;
    [[nodiscard]] const std::string &content() const;
    [[nodiscard]] std::string placeholder() const;

    void setContent(std::string content);

private:
    std::string name_;
    std::string content_;
};

}  // namespace scorehud
