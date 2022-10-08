#ifndef AUXILIARY_H
#define AUXILIARY_H

#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>
#include <unistd.h>
#include <sys/types.h>

#define amount_objects_maximum 	101UL
#define amount_input 	        4UL
#define order                   3UL
#define amount_seconds_sleep    1UL
#define size_file_name          256UL
#define amount_buckets          10UL
#define amount_files            2UL
#define file_index_output       1UL
#define file_index_input        0UL
#define objects_start           1UL
#define seconds_per_hour        3600UL
#define critical_distance       400UL
#define warning_distance_1      2500UL
#define warning_distance_2      1600UL
#define warning_velocity        300UL

#define file_name_input         "input"
#define file_name_output        "output"
#define file_extension          ".json"
#define file_tag( index )           \
index == file_index_input ?         \
file_name_input file_extension :    \
file_name_output file_extension

#define fscanf_data( file, array, o, i, part )                                  \
fscanf( file, format_input( array[i].real[o].part ), &array[i].real[o].part )

#define guess( object, order_2, order_1, order_0, part )    \
make_guess                                                  \
(                                                           \
    ( order_2 ).position.part,                              \
    object->real[order_1].position.part,                    \
    object->real[order_0].position.part                     \
)

#define format_input( T )   \
_Generic                    \
(                           \
    ( T ),                  \
    float:      "%f ",      \
    double:     "%lf ",     \
    default:    "%Lf "      \
)
#define format_output( T )  \
_Generic                    \
(                           \
    ( T ),                  \
    float:      "%f",       \
    double:     "%lf",      \
    default:    "%Lf"       \
)
#define pi_div_180( T )                                 \
_Generic                                                \
(                                                       \
    ( T ),                                              \
    float:      0.0174532925199432957692369076848861F,  \
    double:     0.0174532925199432957692369076848861,   \
    default:    0.0174532925199432957692369076848861L   \
)
#define radius_earth( T )               \
_Generic                                \
(                                       \
    ( T ),                              \
    float:      6371.0087714150004F,    \
    double:     6371.0087714150004,     \
    default:    6371.0087714150004L     \
)
#define to_radians( x ) ( x ) * pi_div_180( x )
#define kilometers_per_meter( T )   \
_Generic                            \
(                                   \
    ( T ),                          \
    float:      0.001F,             \
    double:     0.001,              \
    default:    0.001L              \
)
#define two_point_five( T ) \
_Generic                    \
(                           \
    ( T ),                  \
    float:      2.5F,       \
    double:     2.5,        \
    default:    2.5L        \
)
#define point_five( T ) \
_Generic                \
(                       \
    ( T ),              \
    float:      .5F,    \
    double:     .5,     \
    default:    .5L     \
)

typedef long double units;

/**
 * @brief   Contains latitude, longitude and altitude in units.
 */
struct coordinates
{
    units latitude;
    units longitude;
    units altitude;
};

/**
 * @brief   Contains latitude, longitude, altitude and time in units.
 */
struct data_input
{
    struct coordinates position;
    units time;
};

/**
 * @brief   Contains latitude, longitude, altitude, time of current object (last 3 observations),
 *          their guess counterparts, distance and approach velocity
 *          as well as their guess counterparts, amount of observations and id of current object.
 */
struct object_data
{
    struct data_input   real[order];
    struct data_input   guess;
    units               distance;
    units               approach_velocity;
    units               distance_guess;
    units               approach_velocity_guess;
    size_t              observations;
    size_t              id;
};

extern struct object_data   g_data[amount_objects_maximum + 1];
extern char                 g_files[amount_files][size_file_name];
extern FILE *               g_output;
extern pthread_cond_t       g_condition;
extern pthread_mutex_t      g_mutex;
extern units                g_time;
extern size_t               g_amount_objects;
extern size_t               g_amount_observations;
extern bool                 g_write_happened;

#endif // AUXILIARY_H
