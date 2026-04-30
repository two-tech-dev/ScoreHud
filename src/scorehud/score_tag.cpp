#include "scorehud/score_tag.h"

#include <utility>

namespace scorehud {

ScoreTag::ScoreTag(std::string name, std::string content) : name_(std::move(name)), content_(std::move(content)) {}

const std::string &ScoreTag::name() const
{
    return name_;
}

const std::string &ScoreTag::content() const
{
    return content_;
}

std::string ScoreTag::placeholder() const
{
    return "{" + name_ + "}";
}

void ScoreTag::setContent(std::string content)
{
    content_ = std::move(content);
}

}  // namespace scorehud
