#include "args.h"
#include "global.h"
#include <cstdio>


namespace {
    const char* const usage = R"(Usage:
    bedrock-viz [required parameters] [options]

Required Parameters:
    --db dir                 Directory which holds world files (level.dat is in this dir)

Options:
    --outdir dir             Directory for output file(s). Defaults to "./output/"
    
    --html                   Create html and javascript files to use as a fancy viewer
    --html-most              Create html, javascript, and most image files to use as a fancy viewer
    --html-all               Create html, javascript, and *all* image files to use as a fancy viewer
    --no-tile                Generates single images instead of tiling output into smaller images. May cause loading problems if image size is > 4096px by 4096px
    --tile-size=[width,height] 
                             Changes tile sizes to specified dimensions (Default: 2048px by 2048px)

    Note: [=did] are optional dimension-ids - if not specified, do all dimensions 
    Dimension IDs: 0=Overworld; 1=Nether, 2=End
    Example: --biome=0 1, --biome 0 --biome 1
    --slices[=did]           Create slices (one image for each layer)
    --movie[=did]            Create movie of layers
    --movie-dim x,y,w,h      Integers describing the bounds of the movie (UL X, UL Y, WIDTH, HEIGHT)

    --xml fn                 XML file containing data definitions
    --cfg fn                 CFG file containing parsing configuration

    --detail                 Log extensive details about the world to the log file
    --verbose                verbose output
    --quiet                  supress normal output, continue to output warning and error messages
    --help                   show basic help info
    --help-extended          show extended help info
    --help-experimental      show experimental help info
)";


    const char * const usage_extended = R"(
Extended Options:
    --hide-top=did,bid       Hide a block from top block (did=dimension id, bid=block id)
    --force-top=did,bid      Force a block to top block (did=dimension id, bid=block id)
    --geojson-block=did,bid  Add block to GeoJSON file for use in web app (did=dimension id, bid=block id)

    --schematic[-get] did,x1,y1,z1,x2,y2,z2,fnpart
                             Create a schematic file (fnpart) from (x1,y1,z1) to (x2,y2,z2) in dimension (did)
    Note: [=did] are optional dimension-ids - if not specified, do all dimensions 
    Dimension IDs: 0=Overworld; 1=Nether, 2=End
    Example: --biome=0 1, --biome 0 --biome 1
    --grid[=did]             Display chunk grid on top of images
    --all-image[=did]        Create all image types
    --biome[=did]            Create a biome map image
    --height-col[=did]       Create a height column map image (red is below sea; gray is sea; green is above sea)
    --height-col-gs[=did]    Create a height column map image (grayscale)
    --height-col-alpha[=did] Create a height column map image (alpha)
    --shaded-relief[=did]    Create a shaded relief image
    --slime-chunk[=did]      Create a slime chunk map image

    --no-force-geojson       Don't load geojson in html because we are going to use a web server (or Firefox)
)";


    const char* const usage_experimental = R"(
Experimental Options:
    --shortrun               Debug testing parameter - process only first 1000 records
    --leveldb-filter=i       Bloom filter supposed to improve disk performance (default: 10)
    --leveldb-block-size=i   The block size of leveldb (default: 4096)
    --leveldb-try-repair     If the leveldb fails to open, this will attempt to repair the database. Data loss is possible, use carefully.
)";
}

namespace mcpe_viz{
    void print_usage(char helpFlags)
    {
        printf("%s", usage);
    
        if (helpFlags & HelpFlags::Extended)
            printf("%s", usage_extended);

        if (helpFlags & HelpFlags::Experimental)
            printf("%s", usage_experimental);
    }
}
