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
#include <unordered_set>
#include <cctype>
#include <cassert>


using histogram_t = uint64_t;

static const char TWIST_YOUR_POLUTANTS[] = "twist your polutants";
static const char TWISTYOURPOLUTANTS[] = "twistyourpolutants";


const std::string REV_HASHES[] =
{
    "eb48e309c66cae4483f7722d54b0284e",
    "ef330ba8845cf89bde42c790cca07132",
    "451bb8264faaba8ef26002c0bcb5e566",
};


histogram_t make_histogram(std::string const & s)
{
    histogram_t v{};

    for (auto c : s)
    {
        auto shift = [c]()
            {
                enum {BITSZ = 5};
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

std::string hash_rev_md5(const std::string & phrase)
{
    std::uint8_t hash[MD5_DIGEST_LENGTH];
    char str[MD5_DIGEST_LENGTH * 2 + 1];

    MD5(reinterpret_cast<std::uint8_t const *>(phrase.c_str()), phrase.size(), hash);
    boost::algorithm::hex(hash, hash + sizeof (hash), &str[0]);
    str[sizeof (str) - 1] = 0;
    std::transform(str, str + sizeof (str) - 1, str, ::tolower);

    return std::string(std::rbegin(str) + 1, std::rend(str));
}

bool hash_found(std::string const & rev_hash)
{
    return std::find(std::begin(REV_HASHES), std::end(REV_HASHES), rev_hash) != std::end(REV_HASHES);
}

int main()
{
    std::vector<std::string> const full_words(std::istream_iterator<std::string>(std::cin), std::istream_iterator<std::string>());

    auto re = std::regex("[twistyourpolutants]*");

    std::unordered_multimap<histogram_t, std::string> dict;

    auto const SCORE = make_histogram(TWISTYOURPOLUTANTS);
    auto const NEG = histogram_t{0b100001000010000100001000010000100001000010000100001000010000};

    boost::copy(
        full_words
        | boost::adaptors::filtered([&re](std::string const & s){ return std::regex_match(s, re); })
        | boost::adaptors::uniqued
        | boost::adaptors::filtered([](std::string const & s){ return s.size() < sizeof (TWISTYOURPOLUTANTS);})
        | boost::adaptors::transformed([](std::string const & s){ return std::make_pair(make_histogram(s), s); })
        | boost::adaptors::filtered([&SCORE, &NEG](std::pair<histogram_t, std::string> const & p){ return !((SCORE - p.first) & NEG); })
        ,
        std::inserter(dict, dict.begin())
        );

    std::unordered_set<histogram_t> histograms;
    std::transform(dict.cbegin(), dict.cend(), std::inserter(histograms, histograms.begin()), [](std::pair<histogram_t, std::string> const & p){ return p.first; });

    std::vector<std::string> phrases;

    for (auto const h : histograms)
    {
        if (h == SCORE)
        {
            auto r = dict.equal_range(h);

            std::for_each(r.first, r.second,
                [](std::pair<histogram_t, std::string const> const & p)
                {
                    std::cout << p.second << std::endl;
                });
        }
    }

    std::vector<std::pair<histogram_t, histogram_t>> twosomes;

    for (auto phist : histograms)
    {
        for (auto qhist : histograms)
        {
            if ((phist + qhist) == SCORE)
            {
                twosomes.emplace_back(phist, qhist);
            }
        }
    }

    for (auto const & twosome : twosomes)
    {
        auto prange = dict.equal_range(twosome.first);
        auto qrange = dict.equal_range(twosome.second);

        for (auto p = prange.first; p != prange.second; ++p)
        {
            for (auto q = qrange.first; q != qrange.second; ++q)
            {
                std::cout << p->second << ' ' << q->second << std::endl;
            }
        }
    }

    std::vector<std::tuple<histogram_t, histogram_t, histogram_t>> threesomes;

    for (auto phist : histograms)
    {
        for (auto qhist : histograms)
        {
            for (auto rhist : histograms)
            {
                if ((phist + qhist + rhist) == SCORE)
                {
                    threesomes.emplace_back(phist, qhist, rhist);
                }
            }
        }
    }

    for (auto const & threesome : threesomes)
    {
        auto prange = dict.equal_range(std::get<0>(threesome));
        auto qrange = dict.equal_range(std::get<1>(threesome));
        auto rrange = dict.equal_range(std::get<2>(threesome));

        for (auto p = prange.first; p != prange.second; ++p)
        {
            for (auto q = qrange.first; q != qrange.second; ++q)
            {
                for (auto r = rrange.first; r != rrange.second; ++r)
                {
                    char phrase[sizeof (TWIST_YOUR_POLUTANTS) + 2];
                    sprintf(phrase, "%s %s %s", p->second.c_str(), q->second.c_str(), r->second.c_str());

                    if (hash_found(hash_rev_md5(std::string(phrase))))
                    {
                        std::cout << phrase << std::endl;
                    }
                }
            }
        }
    }

    std::vector<std::tuple<histogram_t, histogram_t, histogram_t, histogram_t>> foursomes;

    for (auto phist : histograms)
    {
        for (auto qhist : histograms)
        {
            for (auto rhist : histograms)
            {
                for (auto shist : histograms)
                {
                    if ((phist + qhist + rhist + shist) == SCORE)
                    {
                        foursomes.emplace_back(phist, qhist, rhist, shist);
                    }
                }
            }
        }
    }

    for (auto const & foursome : foursomes)
    {
        auto prange = dict.equal_range(std::get<0>(foursome));
        auto qrange = dict.equal_range(std::get<1>(foursome));
        auto rrange = dict.equal_range(std::get<2>(foursome));
        auto srange = dict.equal_range(std::get<3>(foursome));

        for (auto p = prange.first; p != prange.second; ++p)
        {
            for (auto q = qrange.first; q != qrange.second; ++q)
            {
                for (auto r = rrange.first; r != rrange.second; ++r)
                {
                    for (auto s = srange.first; s != srange.second; ++s)
                    {
                        char phrase[sizeof (TWIST_YOUR_POLUTANTS) + 3];
                        sprintf(phrase, "%s %s %s %s", p->second.c_str(), q->second.c_str(), r->second.c_str(), s->second.c_str());

                        if (hash_found(hash_rev_md5(std::string(phrase))))
                        {
                            std::cout << phrase << std::endl;
                            return 0;
                        }
                    }
                }
            }
        }
    }

#ifdef BRUTE_FORCE
    std::vector<std::string> const full_words(std::istream_iterator<std::string>(std::cin), std::istream_iterator<std::string>());

    auto re = std::regex("[twistyourpolutants]*");

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
            if (hash_found(hash_rev_md5(p.first)))
            {
                std::cout << p.first << std::endl;
            }
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
                char phrase[sizeof (TWIST_YOUR_POLUTANTS)];

                sprintf(phrase, "%s %s", words[p].c_str(), words[q].c_str());

                if (hash_found(hash_rev_md5(std::string(phrase))))
                {
                    std::cout << phrase << std::endl;
                }
            }
        }
    }

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

                if ((words[p].size() + words[q].size() + words[r].size()) > (sizeof (TWIST_YOUR_POLUTANTS) - 1))
                {
                    break;
                }

                if ((phist + qhist + rhist) == SCORE)
                {
                    char phrase[sizeof (TWIST_YOUR_POLUTANTS) + 2];
                    sprintf(phrase, "%s %s %s", words[p].c_str(), words[q].c_str(), words[r].c_str());

                    if (hash_found(hash_rev_md5(std::string(phrase))))
                    {
                        std::cout << phrase << std::endl;
                    }
                }
            }
        }
    }

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
                        char phrase[sizeof (TWIST_YOUR_POLUTANTS) + 3];
                        sprintf(phrase, "%s %s %s %s", words[p].c_str(), words[q].c_str(), words[r].c_str(), words[s].c_str());

                        if (hash_found(hash_rev_md5(std::string(phrase))))
                        {
                            std::cout << phrase << std::endl;
                            return 0;
                        }
                    }
                }
            }
        }
    }
#endif
}
