#include "parser.h"
#include "server.h"
#include "mainframe.h"

void Parser::parse(const std::string& message, User::UserPtr user) {
	StrVec  split;
	boost::split(split, message, boost::is_any_of(" \t"), boost::token_compress_on);
	boost::to_upper(split[0]);

	if (split[0] == "NICK") {
		if (split.size() < 2) {
			user->session()->sendAsServer(ToString(Response::Error::ERR_NONICKNAMEGIVEN) + " "
				+ user->nick() + " No nickname given" + Config::EOFMessage);
			return;
		}

		if (split[1][0] == '#' || split[1][0] == '&' || split[1][0] == '!' || split[1][0] == '@' || split[1][0] == ':') {
			user->session()->sendAsServer(ToString(Response::Error::ERR_ERRONEUSNICKNAME)
				+ " " + user->nick() + " Wrong nickname format " + Config::EOFMessage);
			return;
		}
		
		user->cmdNick(split[1]);
		user->session()->sendAsServer(ToString(Response::Reply::RPL_NAMREPLY) + " Username changed: " + user->nick()	+ Config::EOFMessage);

	}

	else if (split[0] == "USER") {
		if (split.size() < 5) {
			user->session()->sendAsServer(ToString(Response::Error::ERR_NEEDMOREPARAMS)
				+ Config::EOFMessage);
			return;
		}
		std::string realname = split[4];
		for (unsigned int i = 5; i < split.size(); ++i) {
			realname += " ";
			realname += split[i];
		}
		realname = realname.substr(1);
		user->cmdUser(split[2], realname);

	}

	else if (split[0] == "QUIT") {
		user->cmdQuit();
	}

	else if (split[0] == "JOIN") {
		if (split.size() < 2) return;
		if ((split[1][0] != '#' && split[1][0] != '&') || split[1].size() < 2) return;

		Channel* chan = Mainframe::instance().getChannelByName(split[1]);
		if (chan) {
			if (!chan->hasPass() || (split.size() >= 3 && split[2] == chan->password())) {
				if (!chan->limited() || !chan->full()) {
					user->cmdJoin(chan);
				}
				else {
					user->session()->sendAsServer(ToString(Response::Error::ERR_CHANNELISFULL)
						+ split[1]
						+ " :Cannot join channel (+l)"
						+ Config::EOFMessage);
				}
			}
			else {
				user->session()->sendAsServer(ToString(Response::Error::ERR_BADCHANNELKEY)
					+ split[1]
					+ " :Cannot join channel (+k)"
					+ Config::EOFMessage);
			}
		}
		else {
			chan = new Channel(user, split[1]);
			if (chan) {
				user->cmdJoin(chan);
				Mainframe::instance().addChannel(chan);
			}
		}

	}

	else if (split[0] == "PART") {
		if (split.size() < 2) return;
		Channel* chan = Mainframe::instance().getChannelByName(split[1]);
		if (chan) user->cmdPart(chan);

	}

	else if (split[0] == "TOPIC") {
		if (split.size() >= 2) {
			Channel* chan = Mainframe::instance().getChannelByName(split[1]);
			if (chan) {
				if (split.size() == 2) {
					if (chan->topic().empty()) {
						user->session()->sendAsServer(ToString(Response::Reply::RPL_NOTOPIC)
							+ " " + split[1]
							+ " :No topic is set !"
							+ Config::EOFMessage);
					}
					else {
						user->session()->sendAsServer(ToString(Response::Reply::RPL_TOPIC)
							+ " " + user->nick()
							+ " " + split[1]
							+ " :" + chan->topic()
							+ Config::EOFMessage);
					}
				}
				else if (split.size() >= 3) {
					std::string topic;
					for (unsigned int i = 2; i < split.size(); ++i)  topic += split[i] + " ";
					topic = topic.substr(1);
					if (chan->isOperator(user)) {
						chan->cmdTopic(topic);
						chan->broadcast(user->messageHeader()
							+ "TOPIC "
							+ chan->name()
							+ " :" + chan->topic()
							+ Config::EOFMessage);
					}
				}
			}
		}

	}

	else if (split[0] == "LIST") {
		user->session()->sendAsServer(ToString(Response::Reply::RPL_LISTSTART)
			+ " " + user->nick()
			+ " Channel :Users Name"
			+ Config::EOFMessage);
		ChannelMap channels = Mainframe::instance().channels();
		ChannelMap::iterator it = channels.begin();
		for (; it != channels.end(); ++it) {
			user->session()->sendAsServer(ToString(Response::Reply::RPL_LIST) + " "
				+ user->nick() + " "
				+ it->first + " "
				+ ToString(it->second->userCount()) + " :"
				+ it->second->topic()
				+ Config::EOFMessage);
		}
		user->session()->sendAsServer(ToString(Response::Reply::RPL_LISTEND) + " "
			+ user->nick()
			+ " :End of /LIST"
			+ Config::EOFMessage);

	}

	else if (split[0] == "PRIVMSG") {
		if (split.size() < 3) return;
		std::string message = "";
		for (unsigned int i = 2; i < split.size(); ++i) { message += split[i] + " "; }

		if (split[1][0] == '#' || split[1][0] == '&') {
			Channel* chan = Mainframe::instance().getChannelByName(split[1]);
			if (chan) {
				chan->broadcast(":"
					+ user->nick()
					+ " PRIVMSG "
					+ chan->name() + " "
					+ message + Config::EOFMessage);
			}
		}
		else {
			UserPtr target = Mainframe::instance().getUserByName(split[1]);
			if (target) {
				target->session()->send(":" + user->nick()
					+ " PRIVMSG "
					+ target->nick() + " "
					+ message
					+ Config::EOFMessage);
			}
		}
	}

	else if (split[0] == "KICK") {
		if (split.size() < 3) return;

		Channel* chan = Mainframe::instance().getChannelByName(split[1]);
		UserPtr  victim = Mainframe::instance().getUserByName(split[2]);
		std::string reason = "";
		if (chan && victim) {
			for (unsigned int i = 3; i < split.size(); ++i) {
				reason += split[i] + " ";
			}
			if (chan->isOperator(user) && chan->hasUser(victim) && !chan->isOperator(victim)) {
				user->cmdKick(victim, reason, chan);
				victim->cmdPart(chan);
			}
		}

	}

	else if (split[0] == "PING") { user->cmdPing(); }

	else {
		user->session()->sendAsServer(ToString(Response::Error::ERR_UNKNOWNCOMMAND)
			+ Config::EOFMessage);
	}
}
	
	/*} else if (split[0] == "MODE") {
        if(split.size() < 3) return;

        unsigned char nbParam = 0;
        bool bMode = true;

        Channel* chan = Mainframe::instance()->getChannelByName(split[1]);
        if (!chan)
        {
            user->session()->sendAsServer(ToString(Response::Error::ERR_NOSUCHCHANNEL) + Config::EOFMessage);
            return;
        }

        if (!(chan->hasUser(user)))
        {
            user->session()->sendAsServer(ToString(Response::Error::ERR_NOTONCHANNEL) + Config::EOFMessage);
            return;
        }

        if (!(chan->isOperator(user)))
        {
            user->session()->sendAsServer(ToString(Response::Error::ERR_NOTONCHANNEL) + Config::EOFMessage);
            return;
        }


        for(unsigned int i = 0; i<split[2].size(); ++i)
        {
            if (split[2][i] == '+')
                bMode = true;
            else if (split[2][i] == '-')
                bMode = false;
            else if ((split[2][i] == 'o' && !bMode) || bMode)
                ++nbParam;
            else if ((split[2][i]) != 'k' && split[2][i] != 'l')
            {
                user->session()->sendAsServer(ToString(Response::Error::ERR_UNKNOWNMODE) + Config::EOFMessage);
                return;
            }
        }

        if (nbParam > split.size()-3)
        {
            user->session()->sendAsServer(ToString(Response::Error::ERR_NEEDMOREPARAMS) + Config::EOFMessage);
            return;
        }

        nbParam = 3;

        for(unsigned int i = 0; i<split[2].size(); ++i)
        {
            if (split[2][i] == '+')
                bMode = true;
            else if (split[2][i] == '-')
                bMode = false;
            else if (bMode)
            {
                if (split[2][i] == 'o')
                {
                    User *victim = Mainframe::instance()->getUserByName(split[nbParam]);
                    chan->cmdOPlus(user, victim);
                }
                else if (split[2][i] == 'k')
                {
                    chan->cmdKPlus(split[nbParam]);
                    ++nbParam;
                }
                else if (split[2][i] == 'l')
                {
                    chan->cmdLPlus(split[nbParam]);
                    ++nbParam;
                }
                else
                {
                    user->session()->sendAsServer(ToString(Response::Error::ERR_UNKNOWNMODE) + Config::EOFMessage);
                    return;
                }
            }
            else if (!bMode)
            {
                if (split[2][i] == 'o')
                {
                    User *victim = Mainframe::instance()->getUserByName(split[nbParam]);
                    if (victim->name() == user->name())
                        chan->cmdOMinus(user);
                    ++nbParam;
                }
                else if (split[2][i] == 'k')
                    chan->cmdKMinus();
                else if (split[2][i] == 'l')
                    chan->cmdLMinus();
                else
                {
                    user->session()->sendAsServer(ToString(Response::Error::ERR_UNKNOWNMODE) + Config::EOFMessage);
                    return;
                }
            }
        }*/


