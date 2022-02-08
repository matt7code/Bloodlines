#ifndef STATUSMESSAGE_H
#define STATUSMESSAGE_H

struct StatusMessage {
    StatusMessage(std::string _message = "Status Message.", sf::Time _dura = sf::seconds(10.0f))
        : message(_message)
        , duration(_dura) {}
    std::string     message;
    sf::Time        duration;
};

#endif // STATUSMESSAGE_H
