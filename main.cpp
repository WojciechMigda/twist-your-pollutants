/*******************************************************************************
 * Copyright (c) 2017 Wojciech Migda
 * All rights reserved
 * Distributed under the terms of the MIT License
 *******************************************************************************
 *
 * Filename: main.hpp
 *
 * Description:
 *      'Twist Your Polutants' challenge
 *
 * Authors:
 *          Wojciech Migda (wm)
 *
 *******************************************************************************
 * History:
 * --------
 * Date         Who  Ticket     Description
 * ----------   ---  ---------  ------------------------------------------------
 * 2017-01-05   wm              Initial version
 *
 ******************************************************************************/

#include "boost/range/algorithm/copy.hpp"
#include "boost/range/adaptor/filtered.hpp"
#include "boost/range/adaptor/uniqued.hpp"
#include "boost/range/adaptor/transformed.hpp"
#include "boost/algorithm/hex.hpp"

#include "openssl/md5.h"

#include <iostream>
#include <string>

#include <vector>
#include <iterator>
#include <regex>
#include <cstdint>
#include <utility>
#include <unordered_map>
#include <cctype>
#include <cassert>


using histogram_t = uint64_t;

static const char TWIST_YOUR_POLUTANTS[] = "twist your polutants";
static const char TWISTYOURPOLUTANTS[] = "twistyourpolutants";

histogram_t make_histogram(std::string const & s)
{
    histogram_t v{};

    for (auto c : s)
    {
        enum {BITSZ = 5};
        auto shift = [c]()
            {
                switch (c)
                {
                    case 'p': return 0 * BITSZ;
                    case 'o': return 1 * BITSZ;
                    case 'u': return 2 * BITSZ;
                    case 'l': return 3 * BITSZ;
                    case 't': return 4 * BITSZ;
                    case 'r': return 5 * BITSZ;
                    case 'y': return 6 * BITSZ;
                    case 'w': return 7 * BITSZ;
                    case 'i': return 8 * BITSZ;
                    case 's': return 9 * BITSZ;
                    case 'a': return 10 * BITSZ;
                    case 'n': return 11 * BITSZ;
                }
                assert(0);
                return 16;
            }();
        histogram_t unit = histogram_t(1) << shift;
        v += unit;
    }

    return v;
}

auto out_md5 = [](const std::string & phrase)
{
    std::uint8_t hash[MD5_DIGEST_LENGTH];
    char s[MD5_DIGEST_LENGTH * 2 + 1];

    MD5(reinterpret_cast<std::uint8_t const *>(phrase.c_str()), phrase.size(), hash);
    boost::algorithm::hex(hash, hash + sizeof (hash), &s[0]);
    s[sizeof (s) - 1] = 0;
    std::transform(s, s + sizeof (s) - 1, s, ::tolower);
    std::cout << s << " [" << phrase << ']' << std::endl;
};


int main()
{
    auto re = std::regex("[twistyourpolutants]*");

    std::vector<std::string> const full_words(std::istream_iterator<std::string>(std::cin), std::istream_iterator<std::string>());

    std::unordered_map<std::string, histogram_t> dict;

    auto const SCORE = make_histogram(TWISTYOURPOLUTANTS);
    auto const NEG = histogram_t{0b100001000010000100001000010000100001000010000100001000010000};

    boost::copy(
        full_words
        | boost::adaptors::filtered([&re](std::string const & s){ return std::regex_match(s, re); })
        | boost::adaptors::uniqued
        | boost::adaptors::filtered([](std::string const & s){ return s.size() < sizeof (TWISTYOURPOLUTANTS);})
        | boost::adaptors::transformed([](std::string const & s){ return std::make_pair(s, make_histogram(s)); })
        | boost::adaptors::filtered([&SCORE, &NEG](std::pair<std::string, histogram_t> const & p){ return !((SCORE - p.second) & NEG); })
        ,
        std::inserter(dict, dict.begin())
        );

    std::vector<std::string> words;
    std::transform(dict.cbegin(), dict.cend(), std::back_inserter(words), [](std::pair<std::string, histogram_t> const & p){ return p.first; });

    std::sort(words.begin(), words.end(), [](std::string const & a, std::string const & b){ return a.size() < b.size(); });

    std::vector<std::string> phrases;

    for (auto const & p : dict)
    {
        if (p.second == SCORE)
        {
            std::cout << p.first << std::endl;
            phrases.push_back(p.first);
        }
    }

    for (std::size_t p = 0; p < words.size(); ++p)
    {
        auto phist = dict[words[p]];

        for (std::size_t q = 0; q < words.size(); ++q)
        {
            auto qhist = dict[words[q]];

            if ((phist + qhist) == SCORE)
            {
                std::cout << words[p] << ' ' << words[q] << std::endl;

                char phrase[sizeof (TWIST_YOUR_POLUTANTS)];

                sprintf(phrase, "%s %s", words[p].c_str(), words[q].c_str());
                phrases.push_back(phrase);
//                sprintf(phrase, "%s %s ", words[p].c_str(), words[q].c_str());
//                phrases.push_back(phrase);
//
//                sprintf(phrase, " %s %s", words[p].c_str(), words[q].c_str());
//                phrases.push_back(phrase);
//
//                sprintf(phrase, "%s  %s", words[p].c_str(), words[q].c_str());
//                phrases.push_back(phrase);
            }
        }
    }

//    for (std::size_t p = 0; p < words.size(); ++p)
//    {
//        auto phist = dict[words[p]];
//
//        for (std::size_t q = 0; q < words.size(); ++q)
//        {
//            auto qhist = dict[words[q]];
//
//            if ((SCORE - phist - qhist) & NEG)
//            {
//                continue;
//            }
//
//            if ((words[p].size() + words[q].size()) > (sizeof (TWIST_YOUR_POLUTANTS) - 1))
//            {
//                break;
//            }
//
//            for (std::size_t r = 0; r < words.size(); ++r)
//            {
//                auto rhist = dict[words[r]];
//
//                if ((words[p].size() + words[q].size() + words[r].size()) > (sizeof (TWIST_YOUR_POLUTANTS) - 1))
//                {
//                    break;
//                }
//
//                if ((phist + qhist + rhist) == SCORE)
//                {
//                    std::cout << words[p] << ' ' << words[q] << ' ' << words[r] << std::endl;
//
//                    char phrase[sizeof (TWIST_YOUR_POLUTANTS) + 2];
//                    sprintf(phrase, "%s %s %s", words[p].c_str(), words[q].c_str(), words[r].c_str());
//
//                    phrases.push_back(phrase);
//                }
//            }
//        }
//    }

    for (std::size_t p = 0; p < words.size(); ++p)
    {
        auto phist = dict[words[p]];

        for (std::size_t q = 0; q < words.size(); ++q)
        {
            auto qhist = dict[words[q]];

            if ((SCORE - phist - qhist) & NEG)
            {
                continue;
            }

            if ((words[p].size() + words[q].size()) > (sizeof (TWIST_YOUR_POLUTANTS) - 1))
            {
                break;
            }

            for (std::size_t r = 0; r < words.size(); ++r)
            {
                auto rhist = dict[words[r]];

                if ((SCORE - phist - qhist - rhist) & NEG)
                {
                    continue;
                }

                if ((words[p].size() + words[q].size() + words[r].size()) > (sizeof (TWIST_YOUR_POLUTANTS) - 1))
                {
                    break;
                }

                for (std::size_t s = 0; s < words.size(); ++s)
                {
                    auto shist = dict[words[s]];

                    if ((words[p].size() + words[q].size() + words[r].size() + words[s].size()) > (sizeof (TWIST_YOUR_POLUTANTS) - 1))
                    {
                        break;
                    }

                    if ((phist + qhist + rhist + shist) == SCORE)
                    {
                        std::cout << words[p] << ' ' << words[q] << ' ' << words[r] << ' ' << words[s] << std::endl;

                        char phrase[sizeof (TWIST_YOUR_POLUTANTS) + 3];
                        sprintf(phrase, "%s %s %s %s", words[p].c_str(), words[q].c_str(), words[r].c_str(), words[s].c_str());

                        phrases.push_back(phrase);
                    }
                }
            }
        }
    }


    for (const auto & phrase : phrases)
    {
        out_md5(phrase);
    }
}
