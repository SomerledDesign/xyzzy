#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ARRAY_LEN(array) (sizeof(array) / sizeof((array)[0]))

#ifndef VERSION
#define VERSION "0.7.1"
#endif

typedef struct {
    const char *theme;
    const char *text;
} Omen;

typedef struct {
    int help;
    int list;
    int epic;
    int version;
    int times_explicit;
    size_t times;
    const char *theme;
    int has_seed;
    uint32_t seed;
} Options;

typedef struct {
    uint32_t state;
} Rng;

static const Omen OMENS[] = {
    {"classic", "A hollow voice says, \"Fool.\""},
    {"classic", "Nothing happens, but the room seems offended that you asked."},
    {"classic", "Nothing happens. Maybe another magic word..."},  
    {"maze", "You are in a maze of twisty little passages, all alike."},
    {"maze", "A brass lantern flickers somewhere ahead, then thinks better of it."},
    {"oracle", "The command works best when spoken like you mean it."},
    {"oracle", "A hidden mechanism approves of your persistence."},
    {"treasure", "A velvet-lined compartment slides open, empty except for possibility."},
    {"treasure", "Dust swirls around a pedestal where treasure used to feel important."},
    {"glitch", "Reality lags for a frame, then resumes pretending to be solid."},
    {"glitch", "A nearby wall buffers, redraws, and becomes suspiciously load-bearing again."},
    {"grue", "It is pitch black. You are likely to be eaten by a grue."},
    {"grue", "Two red pixels blink in the dark and reconsider their options."},
    {"epic", "The dungeon notices you noticing it."},
    {"epic", "Somewhere below, a vault door unlocks itself out of respect."},
    {"epic", "An ancient parser accepts your intent and ignores your syntax."}
};

static const char *VALID_THEMES[] = {
    "all", "classic", "maze", "oracle", "treasure", "glitch", "grue", "epic"
};

static int strings_equal(const char *left, const char *right)
{
    return strcmp(left, right) == 0;
}

static int is_valid_theme(const char *theme)
{
    size_t index;

    for (index = 0; index < ARRAY_LEN(VALID_THEMES); ++index) {
        if (strings_equal(theme, VALID_THEMES[index])) {
            return 1;
        }
    }

    return 0;
}

static int parse_u32(const char *text, uint32_t *value)
{
    uint64_t parsed = 0;
    size_t index;

    if (text == NULL || text[0] == '\0') {
        return 0;
    }

    for (index = 0; text[index] != '\0'; ++index) {
        const unsigned char ch = (unsigned char) text[index];
        if (ch < '0' || ch > '9') {
            return 0;
        }

        parsed = (parsed * 10u) + (uint64_t) (ch - '0');
        if (parsed > UINT32_MAX) {
            return 0;
        }
    }

    *value = (uint32_t) parsed;
    return 1;
}

static void print_usage(FILE *stream, const char *program_name)
{
    fprintf(stream,
        "Usage: %s [--epic] [--theme THEME] [--times COUNT] [--seed VALUE]\n"
        "       %s --list [--theme THEME]\n"
        "       %s --help | --version\n\n"
        "Themes: all, classic, maze, oracle, treasure, glitch, grue, epic\n",
        program_name, program_name, program_name);
}

static int parse_arguments(int argc, char **argv, Options *options, const char **bad_argument, const char **error)
{
    int index;

    for (index = 1; index < argc; ++index) {
        const char *argument = argv[index];

        if (strings_equal(argument, "--help") || strings_equal(argument, "-h")) {
            options->help = 1;
            continue;
        }

        if (strings_equal(argument, "--list")) {
            options->list = 1;
            continue;
        }

        if (strings_equal(argument, "--epic")) {
            options->epic = 1;
            continue;
        }

        if (strings_equal(argument, "--version")) {
            options->version = 1;
            continue;
        }

        if (strings_equal(argument, "--theme")) {
            if (index + 1 >= argc) {
                *error = "--theme requires a value";
                return 0;
            }

            options->theme = argv[++index];
            if (!is_valid_theme(options->theme)) {
                *bad_argument = options->theme;
                *error = "unknown theme";
                return 0;
            }

            continue;
        }

        if (strings_equal(argument, "--times")) {
            uint32_t parsed_times = 0;

            if (index + 1 >= argc) {
                *error = "--times requires a positive integer";
                return 0;
            }

            if (!parse_u32(argv[++index], &parsed_times) || parsed_times == 0u) {
                *error = "--times requires a positive integer";
                return 0;
            }

            options->times = (size_t) parsed_times;
            options->times_explicit = 1;
            continue;
        }

        if (strings_equal(argument, "--seed")) {
            uint32_t parsed_seed = 0;

            if (index + 1 >= argc) {
                *error = "--seed requires an integer";
                return 0;
            }

            if (!parse_u32(argv[++index], &parsed_seed)) {
                *error = "--seed requires an integer in the range 0..4294967295";
                return 0;
            }

            options->has_seed = 1;
            options->seed = parsed_seed;
            continue;
        }

        *bad_argument = argument;
        *error = "unknown option";
        return 0;
    }

    if (options->epic && !options->times_explicit) {
        options->times = 3u;
    }

    return 1;
}

static void rng_seed(Rng *rng, uint32_t seed)
{
    rng->state = seed;
    if (rng->state == 0u) {
        rng->state = 0x6D2B79F5u;
    }
}

static uint32_t rng_next(Rng *rng)
{
    uint32_t value = rng->state;

    value ^= value << 13;
    value ^= value >> 17;
    value ^= value << 5;
    rng->state = value;
    return value;
}

static size_t rng_bounded(Rng *rng, size_t upper_bound)
{
    return (size_t) (rng_next(rng) % (uint32_t) upper_bound);
}

static void build_pool(const char *theme, size_t *pool, size_t *pool_count)
{
    size_t index;
    size_t count = 0;

    for (index = 0; index < ARRAY_LEN(OMENS); ++index) {
        if (strings_equal(theme, "all") || strings_equal(theme, OMENS[index].theme)) {
            pool[count++] = index;
        }
    }

    *pool_count = count;
}

static void shuffle_pool(size_t *pool, size_t pool_count, Rng *rng)
{
    size_t index;

    if (pool_count < 2u) {
        return;
    }

    for (index = pool_count - 1u; index > 0u; --index) {
        const size_t other = rng_bounded(rng, index + 1u);
        const size_t temp = pool[index];
        pool[index] = pool[other];
        pool[other] = temp;
    }
}

static void print_epic_header(const Options *options)
{
    printf("== XYZZY ==\n");
    printf("Theme: %s\n", options->theme);
    printf("Echoes: %zu\n", options->times);
    if (options->has_seed) {
        printf("Seed: %u\n", options->seed);
    }
    putchar('\n');
}

static void print_epic_footer(void)
{
    printf("\nThe maze remembers.\n");
}

static uint32_t default_seed(void)
{
    uint32_t seed = (uint32_t) time(NULL);
    seed ^= (uint32_t) clock();
    if (seed == 0u) {
        seed = 0xA341316Cu;
    }
    return seed;
}

static void print_listing(const size_t *pool, size_t pool_count)
{
    size_t index;

    for (index = 0; index < pool_count; ++index) {
        const Omen *omen = &OMENS[pool[index]];
        printf("[%s] %s\n", omen->theme, omen->text);
    }
}

static void print_selected_omens(const Options *options, const size_t *pool, size_t pool_count, Rng *rng)
{
    size_t output_index;

    if (options->epic) {
        print_epic_header(options);
    }

    for (output_index = 0; output_index < options->times; ++output_index) {
        size_t pool_index = output_index;
        const Omen *omen;

        if (pool_index >= pool_count) {
            pool_index = rng_bounded(rng, pool_count);
        }

        omen = &OMENS[pool[pool_index]];
        if (options->epic) {
            printf("%zu. [%s] %s\n", output_index + 1u, omen->theme, omen->text);
        } else {
            printf("%s\n", omen->text);
        }
    }

    if (options->epic) {
        print_epic_footer();
    }
}

int main(int argc, char **argv)
{
    Options options;
    const char *bad_argument = NULL;
    const char *error = NULL;
    size_t pool[ARRAY_LEN(OMENS)];
    size_t pool_count = 0;
    Rng rng;

    options.help = 0;
    options.list = 0;
    options.epic = 0;
    options.version = 0;
    options.times_explicit = 0;
    options.times = 1u;
    options.theme = "all";
    options.has_seed = 0;
    options.seed = 0u;

    if (!parse_arguments(argc, argv, &options, &bad_argument, &error)) {
        if (error != NULL && bad_argument != NULL) {
            fprintf(stderr, "xyzzy: %s '%s'\n", error, bad_argument);
        } else if (error != NULL) {
            fprintf(stderr, "xyzzy: %s\n", error);
        }
        print_usage(stderr, argv[0]);
        return 1;
    }

    if (options.help) {
        print_usage(stdout, argv[0]);
        return 0;
    }

    if (options.version) {
        printf("xyzzy %s\n", VERSION);
        printf("Copyright (c) 1998 Somerled Design.\n");
        printf("GNU General Public License, version 2.\n");
        return 0;
    }

    build_pool(options.theme, pool, &pool_count);
    if (pool_count == 0u) {
        fprintf(stderr, "xyzzy: no omens available for theme '%s'\n", options.theme);
        return 1;
    }

    if (options.list) {
        print_listing(pool, pool_count);
        return 0;
    }

    rng_seed(&rng, options.has_seed ? options.seed : default_seed());
    shuffle_pool(pool, pool_count, &rng);
    print_selected_omens(&options, pool, pool_count, &rng);

    return 0;
}
