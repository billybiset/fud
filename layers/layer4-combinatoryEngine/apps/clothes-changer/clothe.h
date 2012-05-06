/**
 *  @file:      clothe.h
 *  @details    Header file for Clothe class.\n
 *              System: CombEng \n
 *              Language: C++\n
 *
 *  @author     Favio Bettiol
 *  @email      favio.bettiol AT gmail.com
 *
 *  @author     Diego Diaz
 *  @email      diazdiego86 AT gmail.com
 *
 *  @date       August 2010
 *  @version    0.1
 *
 * This file is part of CombEng
 *
 * CombEng : Combinatory Engine, an FuD's abstraction layer for problems that
 * require to combine elements. Also, this layer provides ranking and prune
 * features for these combinations.
 * <http://fud.googlecode.com/>
 *
 * Copyright (C) 2010 - Favio Bettiol and Diego Diaz
 *
 * CombEng is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CombEng is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CombEng .  If not, see <http://www.gnu.org/licenses/>.
 *
 */

//=================================
// include guard
#ifndef _CLOTHE_H
#define _CLOTHE_H

//=================================
// include dependencies
#include <string>
#include <mili/mili.h>

class Clothe
{
public:
    Clothe()
    {}

    Clothe(const std::string& nomb, const std::string& color)
        : _name(nomb), _colour(color)
    {}

    inline bool operator == (const Clothe& other) const
    {
        return ((_name == other._name) && (_colour == other._colour));
    }

    inline std::string get_name() const
    {
        return _name;
    }

    inline std::string get_colour() const
    {
        return _colour;
    }

    bool operator < (const Clothe& other) const
    {
        return (_name < other.get_name());
    }


    friend inline mili::bostream<>& operator<< (mili::bostream<>& bos, const Clothe& clo)
    {
        bos << clo._name;
        bos << clo._colour;

        return bos;
    }

    friend inline mili::bistream<>& operator>> (mili::bistream<>& bis, Clothe& clo)
    {
        std::string aux;
        bis>>clo._name;
        bis>>clo._colour;
        return bis;
    }

    inline Clothe operator = (const Clothe& clo)
    {
        Clothe ret (clo._name,clo._colour);
        return ret;
    }

    void print_obj()
    {
        std ::cout << this->get_name() << " " << this->get_colour() << " ";
    }

private:
    std::string _name;
    std::string _colour;
};


#endif  /* _CLOTHE_H */
