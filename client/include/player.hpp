/**
 * @file player.hpp
 * @author Yann Le Masson
 * 
 */
#ifndef PLAYER_HPP
#define PLAYER_HPP

namespace ASE
{
    
template<typename PlayerDataStructure>
class Player
{
private:
    int id_;
    PlayerDataStructure data_;

public:
    Player(int id) : id_(id)
    {

    }

    int getId() const
    {
        return id_;
    }

    PlayerDataStructure &getDataRef()
    {
        return data_;
    }
    
    
};
  


} // namespace ASE

#endif