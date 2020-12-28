/*
  Legend generator - takes in bedrock_viz.xml and spits out bedrock_viz_legend.html
  (c) cabbey 2020

  GPL'ed code - see LICENSE

 */
#include <cstdio>
#include <filesystem>
#include <string>
#include <cstdint>
#include <iostream>

#include <leveldb/db.h>
#include <leveldb/cache.h>

#include <boost/program_options.hpp>
#include <fstream>

#include "define.h"
#include "util.h"
#include "asset.h"
#include "args.h"
#include "control.h"
#include "utils/unknown_recorder.h"
#include "world/world.h"
#include "utils/fs.h"
#include "global.h"
#include "xml/loader.h"
#include "minecraft/v2/biome.h"
#include "minecraft/v2/block.h"


using namespace boost::program_options;
using namespace std;


int main(int argc, char** argv)
{
    using namespace mcpe_viz;

    auto ret = load_xml(data_path("bedrock_viz.xml").generic_string());
    if (ret != 0) {
        log::error("Failed to load xml file");
        return -1;
    }

    ofstream output;
    const string &outputFilename = static_path("bedrock_viz_legend.html").generic_string();
    output.open(outputFilename);

    output << "<html >" << endl
           << "    <head>" << endl
           << "        <title>" << endl
           << "            Bedrock Viz Color Legend" << endl
           << "        </title>" << endl
           << "        <style>" << endl
           << "body {" << endl
           << "    color: #e0e0e0;" << endl
           << "    background-color: #101010;" << endl
           << "}" << endl
           << endl
           << ".grid-container {" << endl
           << "    display: grid;" << endl
           << "    grid-gap: 1em;" << endl
           << "}" << endl
           << endl
           << ".grid-cell {" << endl
           << "    background-color: #ffffff;" << endl
           << "    color: #000000;" << endl
           << "    width: 26em;" << endl
           << "    height: 6em;" << endl
           << "}" << endl
           << endl
            << ".with-variants {" << endl
            << "    height: 11em;" << endl
            << "}" << endl
            << endl
           << ".color-swatch {" << endl
           << "    display: block;" << endl
           << "    width: 4em;" << endl
           << "    height: 4em;" << endl
           << "    margin-top: 1em;" << endl
           << "    margin-left: 21em; /* 5 less than cell width */" << endl
           << "    box-shadow: 0.2em 0.2em #888888;" << endl
           << "}" << endl
           << endl
           << ".name {" << endl
           << "    float: left;" << endl
           << "    margin-left: 0.5em;" << endl
           << "    margin-top: 0.5em;" << endl
           << "    font-size: 2em;" << endl
           << "    max-width: 18em; /* 1 less than color swatch's left margin */" << endl //being ignored??
           << "}" << endl
           << endl
           << ".variant-swatch {" << endl
           << "    display: block;" << endl
           << "    float: left;" << endl
           << "    width: 2em;" << endl
           << "    height: 2em;" << endl
           << "    margin: 0.25em;" << endl
           << "    box-shadow: 0.1em 0.1em #888888;" << endl
           << "}" << endl
           << endl
           << ".new-row {" << endl
           << "    clear: left;" << endl
           << "    margin-left: 0.75em;" << endl
           << "}" << endl
           << endl
           << "        </style>" << endl
           << "    </head>" << endl
           << "    <body>" << endl
           << "        <h1>Bedrock Viz Color Legend</h1>" << endl
           << "        <a name=\"biomes\"></a>" << endl
           << "        <h2>Biomes</h2>" << endl
           << "        <div class=\"grid-container\">";

    unordered_map<Colored::ColorType, string> biomeColorMap;

    for(auto& biome : mcpe_viz::Biome::list()) {
        if (biome->is_color_set()) {
            // check for conflicts...
            auto found = biomeColorMap.find(biome->color());
            if (found != biomeColorMap.end()) {
                log::warn("duplicate biome color found, {} and {}", biome->name, found->second);
            } else {
                // log that we've seen it
                biomeColorMap[biome->color()] = biome->name;
                // output legend entry
                output << "<div class=\"grid-cell\">" << endl
                       << "    <div class=\"name\">" << escapeString(biome->name, "'") << "</div>" << endl
                       << "    <div class=\"color-swatch\" style=\"background-color: #";
                // set the stream to render integers correctly in hexcodes for colors
                output << hex << setfill('0') << setw(6) << right << local_be32toh(biome->color());
                output << "\">&nbsp;</div>" << endl
                       << "</div>" << endl;
            }
        } else {
            log::error("Biome id:{} name:{} has no color", biome->id, biome->name);
        }
    }

    output << "        </div>" << endl //close container
           << "        <a name=\"blocks\"></a>" << endl
           << "        <h2>Blocks</h2>" << endl
           << "        <div class=\"grid-container\">";

    unordered_map<Colored::ColorType, string> blockColorMap;

    for(auto& block : mcpe_viz::Block::list()) {
        bool hasVariants = block->hasVariants();

        if (block->is_color_set()) {
            // check for conflicts...
            auto found = blockColorMap.find(block->color());
            if (found != blockColorMap.end()) {
                log::warn("duplicate block color found, {} and {}", block->name, found->second);
            } else {
                // log that we've seen it
                blockColorMap[block->color()] = block->name;
            }

            if (hasVariants) {
                output << "<div class=\"grid-cell with-variants\">" << endl;
            } else {
                output << "<div class=\"grid-cell\">" << endl;
            }
            output << "    <div class=\"name\">" << escapeString(block->name, "'") << "</div>" << endl
                   << "    <div class=\"color-swatch\" style=\"background-color: #";
            // set the stream to render integers correctly in hexcodes for colors
            output << hex << setfill('0') << setw(6) << right << local_be32toh(block->color());
            output << "\" title=\"" << block->name << "\">&nbsp;</div>" << endl;

            if (hasVariants) {
                int i = 0;
                for(auto &variant : block->getVariants()) {
                    auto found = blockColorMap.find(variant.second->color());
                    if (found != blockColorMap.end()) {
                        log::warn("duplicate block color found, {} and {}", variant.second->name, found->second);
                    } else {
                        // log that we've seen it
                        blockColorMap[variant.second->color()] = variant.second->name;
                    }

                    if (i++ % 8 == 0) {
                        output << "    <div class=\"variant-swatch new-row\" style=\"background-color: #";
                    } else {
                        output << "    <div class=\"variant-swatch\" style=\"background-color: #";
                    }
                    output << hex << setfill('0') << setw(6) << right << local_be32toh(variant.second->color());
                    output << "\" title=\"" << block->getVariantName(variant.first) << "\">&nbsp;</div>" << endl;
                }
            }

            output << "</div>" << endl; //end grid-cell
        } else {
            log::error("Block id:{} name:{} has no color", block->id, block->name);
        }
    }


    output << "</body></html>" << endl;

    output.close();

    log::info("Wrote {}", outputFilename);

    return 0;
}
