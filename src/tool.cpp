#include "tool.h"

float Tool::dist(const sf::Vector2f& p1, const sf::Vector2f& p2) {
    return std::sqrt(std::pow(p2.x - p1.x, 2.0f) + std::pow(p2.y - p1.y, 2.0f));
}

float Tool::dist(const sf::Vector3i& p1, const sf::Vector3i& p2) {
    return std::sqrt(std::pow(p2.x - p1.x, 2.0f) + std::pow(p2.y - p1.y, 2.0f));
}

float Tool::dist(const sf::Vector3f& p1, const sf::Vector3f& p2) {
    return std::sqrt(std::pow(p2.x - p1.x, 2.0f) + std::pow(p2.y - p1.y, 2.0f));
}

sf::Vector2f Tool::centerSprite(sf::Sprite& sprite) {
    sprite.setOrigin(sf::Vector2f(sprite.getLocalBounds().height / 2, sprite.getLocalBounds().width / 2));
    return sf::Vector2f(sprite.getLocalBounds().height / 2, sprite.getLocalBounds().width / 2);
}

sf::Vector2f Tool::convertToTileCoords(sf::Vector2f& worldPos) {
    int x = worldPos.x;
    int y = worldPos.y;

    x /= TILE_SIZE_X;
    y /= TILE_SIZE_Y;

    x *= TILE_SIZE_X;
    y *= TILE_SIZE_Y;

    return sf::Vector2f(x, y);
}

std::string Tool::pad(std::string in, int amt) {
    int n = amt - in.size();
    if(n < 1) {
        in.resize(amt);
        return in;
    }
    std::string space(n, ' ');
    return (space + in);
}

std::string Tool::padLeft(std::string in, int amt) {
    int n = amt - in.size();
    if(n < 1) {
        in.resize(amt);
        return in;
    }
    std::string space(n, ' ');
    return (in + space);
}

std::string Tool::padNum(long in, int amt) {
    std::stringstream ss;
    ss << in;
    int n = amt - ss.str().size();

    if(n < 1)
        return ss.str();

    std::string space(n, ' ');
    return (space + ss.str());
}

int Tool::getRand(const int & min, const int & max) {
    if (min == max) return min;
    if (min > max) {
        std::cerr << "int Tool::getRand(const int & min, const int & max): Min greater than Max." << std::endl;
    }
    static std::random_device r;
    static std::mt19937 generator(r());

    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
}

std::string Tool::FormatLine(const std::string text, const int max_length) {
    int len = 0;
    std::string ret;
    std::size_t last_space = 0;

    for (std::size_t i = 0; i < text.size(); ++i) {
        if (text[i] == ' ') last_space = i;

        ret += text[i];
        len ++;

        if (len > max_length) {
            // Back up to last space.
            i = i - (i - last_space);
            ret.resize(i);
            ret += "\n";
            len = 0;
        }
    }
    return ret;
}

void Tool::FormatLineIntoVector(const std::string& text, const int max_length, std::vector<std::string>& vecref, bool clear_vector) {
    std::string ret;
    std::size_t last_space = 0;
    if (clear_vector) vecref.clear();

    for (std::size_t i = 0; i < text.size(); ++i) {
        if (text[i] == ' ') last_space = i;
        ret += text[i];
        if ((int)ret.size() > max_length) {
            for (unsigned j = 0; j <= (i - last_space); j++) ret.pop_back(); // back up
            i = last_space;
            vecref.push_back(ret);
            ret.clear();
        }
    }
    vecref.push_back(ret);
}

std::string Tool::reconstituteString(const std::vector<std::string>& vec) {
    if(vec.empty()) return "Error: Empty vector fed to Dialog::reconstituteString";
    std::string ret;
    for (const auto &word : vec) {
        ret += word;
        ret.push_back(' ');
    }
    ret.pop_back();
    return ret;
}

std::string Tool::Trim(const std::string& str, const std::string& whitespace) {
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos) return ""; // no content
    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin;
    //const auto strRange = strEnd - strBegin;
    //std::cout << "Trim Debug: Original String: '" << str << "' Trimmed: '" << str.substr(strBegin, strRange + 2) << "'" << std::endl;
    return str.substr(strBegin, strRange + 2);
}

std::vector<std::string> Tool::Tokenize(const std::string& str) {
    std::string tmp;
    std::vector<std::string> vec;
    //std::cout << "Tool Check: str = \"" << str << "\"" << std::endl;
    //if (str.empty()) return vec;
    for (const auto& c : str) {
        if (c != ' ') {
            tmp.push_back(c);
        } else {
            vec.push_back(tmp);
            tmp.clear();
        }
    }
    vec.push_back(tmp);
    return vec;
}

std::string Tool::generateName(const int len) {
    std::string ret;
    const std::string cons = "bcdfghjklmnprst"; // 15
    const std::string vols = "aaaeeeeeioou"; // 5

    ret += cons[Tool::getRand(0, cons.size() - 1)];
    ret += vols[Tool::getRand(0, vols.size() - 1)];
    ret += cons[Tool::getRand(0, cons.size() - 1)];
    ret += vols[Tool::getRand(0, vols.size() - 1)];
    ret += cons[Tool::getRand(0, cons.size() - 1)];
    ret += vols[Tool::getRand(0, vols.size() - 1)];
    ret += cons[Tool::getRand(0, cons.size() - 1)];
    ret += vols[Tool::getRand(0, vols.size() - 1)];
    ret += cons[Tool::getRand(0, cons.size() - 1)];
    ret += vols[Tool::getRand(0, vols.size() - 1)];
    ret += cons[Tool::getRand(0, cons.size() - 1)];
    ret += vols[Tool::getRand(0, vols.size() - 1)];
    ret += cons[Tool::getRand(0, cons.size() - 1)];
    ret += vols[Tool::getRand(0, vols.size() - 1)];

    if (len == -1) ret.resize(Tool::getRand(3, 14));

    ret[0] = std::toupper(ret[0]);
    return ret;
}

std::string Tool::TimeToString(int seconds) {
    std::stringstream ss;
    long t = seconds;
    int s = t % 60;
    int m = (t / 60) % 60;
    int h = (t / 3600) % 24;
    int d = (t / 86400) % 7;
    int w = (t / 604800);

    if (w > 0) ss << w << "w";
    if (d > 0) ss << d << "d";
    if (h > 0) ss << h << "h";
    if (m > 0) ss << (m < 10 ? "0" : "") << m << "m";
    if (s > 0) ss << (s < 10 ? "0" : "") << s << "s";
    return ss.str();
}

int Tool::GetDirList(std::string dir, std::vector<std::string> &files) {
    DIR *dp;
    struct dirent *dirp;
    if((dp = opendir(dir.c_str())) == NULL) {
        std::cerr << "Error #" << errno << " opening " << dir << "." << std::endl;
        return errno;
    }
    while ((dirp = readdir(dp)) != NULL) {
        std::string filename = dirp->d_name;
        if (filename == "." || filename == "..") continue;
        files.push_back(filename);
    }
    closedir(dp);
    return 0;
}

std::string Tool::GetCurrentDateAndTime() {
    time_t timer;
    struct tm * ti;
    const char * weekday[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    std::time(&timer);
    std::stringstream ss;
    ti = std::localtime( &timer );
    int hr = ti->tm_hour;
    if (hr > 12) hr -= 12;
    if (hr == 0) hr = 12;

    int mn = ti->tm_min;
    int sc = ti->tm_sec;

    ss << weekday[ti->tm_wday] << (hr < 10 ? "  " : " ") << hr << ":"
       << (mn < 10 ? "0" : "") << mn << ":"
       << (sc < 10 ? "0" : "") << sc;

    return ss.str();
}

std::string Tool::Clip(const std::string text, const int max_length) {
    std::string ret = text;
    ret.resize(max_length);
    return ret;
}

std::string Tool::ConvertMsToTimeString(const long ms) {
    int t = ms / 1000;

    int s = t % 60;
    int m = (t / 60) % 60;
    int h = (t / 3600) % 24;
    int d = (t / 86400) % 7;
    int w = (t / 604800);

    std::stringstream ss;
    if (w > 0) ss << w << (w < 2 ? " week, " : " weeks, ");
    if (d > 0) ss << d << (d < 2 ? " day, " : " days, ");
    if (h > 0) ss << h << (h < 2 ? " hour, " : " hours, ");
    if (m > 0) ss << m << (m < 2 ? " minute, " : " minutes, ");
    if (s > 0) ss << s << (s < 2 ? " second " : " seconds ");

    return ss.str();
}

sf::Vector2i Tool::ConvertClickToScreenPos(const sf::Vector2i click, const sf::RenderWindow& window) {
    return sf::Vector2i((1920.0f / window.getSize().x) * click.x, (1080.0f / window.getSize().y) * click.y);
}

int Tool::face(sf::Vector2f from, sf::Vector2f to) {
    int facing = DIR_SOUTH;

    sf::Vector2f m_facingVec = to - from;

    if (std::abs(m_facingVec.x) > std::abs(m_facingVec.y)) {
        if(m_facingVec.x > 0) facing = DIR_EAST;
        else facing = DIR_WEST;
    } else {
        if(m_facingVec.y > 0) facing = DIR_SOUTH;
        else facing = DIR_NORTH;
    }
    return facing;
}

int Tool::getDigitAt(int num, int pos) {
    return (num / (int)std::pow(10.0f, pos)) % 10;
}

int Tool::getNumDigits(int x) {
    x = std::abs(x);
    return
           (x < 10 ? 1 :
            (x < 100 ? 2 :
             (x < 1000 ? 3 :
              (x < 10000 ? 4 :
               (x < 100000 ? 5 :
                (x < 1000000 ? 6 :
                 (x < 10000000 ? 7 :
                  (x < 100000000 ? 8 :
                   (x < 1000000000 ? 9 :
                    10)))))))));
}

bool Tool::LoadVecStr(const std::string& title, std::vector<std::string>& vec, bool trim) {
    vec.clear();
    std::ifstream in(title);
    if (!in) return false;

    std::string input;
    while (std::getline(in, input)) {
        if(input[0] == '#') continue;
        if(input.empty()) continue;
        if(trim) Tool::Trim(input);
        vec.push_back(input);
    }
    in.close();
    return true;
}

void Tool::DumpVecStr(const std::vector<std::string>& vec) {
    std::cout << "=[ VecStr Dump: ]===================================" << std::endl;
    for (const auto& line : vec) std::cout << line << std::endl;
    std::cout << "=[ Done. ]============================================" << std::endl;
}

void Tool::ConvertClickToScreenPos(sf::Vector2i& click, sf::RenderWindow& window) {
    float x = (1920.0f / window.getSize().x) * click.x;
    float y = (1080.0f / window.getSize().y) * click.y;
    click = sf::Vector2i(x, y);
}

/// @desc Returns a vector of strings containing each part of input.
///       Empty sections insert an empty string into the vector.
std::vector<std::string>Tool::Parse(std::string input, const char delim)
{
    std::vector<std::string> ret;
    std::size_t pos;
    do {
        pos = input.find(delim, 0);
        ret.push_back(input.substr(0, pos));
        input = input.substr(pos + 1, std::string::npos);
    } while (pos != std::string::npos);
    return ret;
}
