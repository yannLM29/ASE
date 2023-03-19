/**
 * @file player_list.hpp
 * @author Yann Le Masson
 * 
 */
#ifndef PLAYER_LIST_HPP
#define PLAYER_LIST_HPP

#include <list>
#include <algorithm>
#include <string>
#include <exception>

#include "player.hpp"

namespace ASE
{

template<typename PlayerDataStructure>
class PlayerList
{
private:
    std::list<Player<PlayerDataStructure>> all_players_;
public:
    PlayerList(/* args */)
    {

    }

    /**
     * @brief Get an iterator to player selected by id
     * 
     * @param id 
     * @return Player Iterator
     */
    auto getPlayerItById(int id)
    {
        return std::find_if(all_players_.begin(), all_players_.end(), [&](auto &player){
            return player.getId() == id;
        });


    }

    auto getPlayerListRef()
    {
        return std::ref(all_players_);
    }

    /**
     * @brief add a player to the player_list with id player_id
     * 
     * @param player_id 
     */
    void addPlayer(int player_id)
    {
        all_players_.emplace_back(player_id);
    }

    /**
     * @brief remove the player with id player_id from the player_list 
     * 
     * @param player_id 
     */
    void removePlayer(int player_id)
    {
        all_players_.remove_if([&](const auto &player){
            return player.getId() == player_id;
        });
    }

    std::string toString()
    {
        std::string output = "client list of length " + std::to_string(all_players_.size()) + std::string(": ");
    
        std::for_each(all_players_.cbegin(), all_players_.cend(),[&](const auto &player){
            output += std::to_string(player.getId()) + " ";
        });
        
        return output;
    }

};



} // namespace ASE

#endif