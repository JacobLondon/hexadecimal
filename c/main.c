#include "convert.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

int main(int argc, char **argv)
{
    size_t i;
    ConvertResult res;
    int rv = 0;

    struct {
        const char *value;
        double expected;
        int expected_rv;
    } float_cases[] = {
        {"1", 0, 1},

        {"1.2e3", 1200.0, 0},
        {"1.2E3", 1200.0, 0},
        {"1e3", 1000.0, 0},
        {"1e-3", 0.001, 0},
        {"-1.e-3", -0.001, 0},
        {"-1.E3", -1000.0, 0},
        {".1e3", 100.0, 0},
        {".1E3", 100.0, 0},

        {"1.2", 1.2, 0},
        {"-1.2", -1.2, 0},
        {"-.1", -.1, 0},
        {".1", .1, 0},
    };

    if (argc == 2) {
        res = ConvertString(argv[1]);
        if (res.type >= CONVERT_RESULT_ERROR) {
            fprintf(stderr, "Failed to convert: %s: ", argv[1]);
            return 1;
        }
        ConvertResultDump(&res, stdout);
        printf("\n");
        return 0;
    }

    for (i = 0; i < ARRAY_SIZE(float_cases); i++) {
        res = ConvertString(float_cases[i].value);

        if ((res.real != float_cases[i].expected) &&
            ((res.type >= CONVERT_RESULT_ERROR) && (float_cases[i].expected_rv != 1)))
        {
            fprintf(stderr, "Failed to convert \"%s\" to %lf (%lf)\n",
                float_cases[i].value, float_cases[i].expected, res.real);
            rv += 1;
        }
        else
        {
            ConvertResultDump(&res, stdout);
            printf("\n");
        }
    }

    return rv;
}
