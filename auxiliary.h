#ifndef AUXILIARY_H
#define AUXILIARY_H

#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define amount_objects_maximum 	100U
#define amount_input 	        4U
#define order                   (2U + 1U)
#define amount_seconds_sleep    1U
#define size_file_name          256U
#define amount_files            2U
#define file_index_output       1U
#define file_index_input        0U

#define file_name_input         "input"
#define file_name_output        "output"
#define file_extension          ".json"
#define file_tag( index )           \
index == file_index_input ?         \
file_name_input file_extension :    \
file_name_output file_extension

#define fscanf_data( array, o, i, part )   \
fscanf( data, format_input( array[o][i].part ), &array[o][i].part )

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
#define PI_( T )                \
_Generic                        \
(                               \
    ( T ),                      \
    float:      acosf( -1.F ),  \
    double:     acos( -1. ),    \
    default:    acosl( -1.L )   \
)
#define radius_earth( T )   \
_Generic                    \
(                           \
    ( T ),                  \
    float:      6371.0088F, \
    double:     6371.0088,  \
    default:    6371.0088L  \
)
#define to_radians( x ) ( x ) * PI_( x ) / 180
#define cos_( T )               \
_Generic                        \
(                               \
    ( T ),                      \
    float:      cosf( ( T ) ),  \
    double:     cos( ( T ) ),   \
    default:    cosl( ( T ) )   \
)
#define sin_( T )               \
_Generic                        \
(                               \
    ( T ),                      \
    float:      sinf( ( T ) ),  \
    double:     sin( ( T ) ),   \
    default:    sinl( ( T ) )   \
)
#define sqrt_( T )              \
_Generic                        \
(                               \
    ( T ),                      \
    float:      sqrtf( ( T ) ), \
    double:     sqrt( ( T ) ),  \
    default:    sqrtl( ( T ) )  \
)
#define acos_( T )              \
_Generic                        \
(                               \
    ( T ),                      \
    float:      acosf( ( T ) ), \
    double:     acos( ( T ) ),  \
    default:    acosl( ( T ) )  \
)
#define critical_distance_base( T ) \
_Generic                            \
(                                   \
    ( T ),                          \
    float:      20.F,               \
    double:     20.,                \
    default:    20.L                \
)
#define critical_distance( T )  \
critical_distance_base( T ) * critical_distance_base( T )
#define warning_distance_1_base( T )    \
_Generic                                \
(                                       \
    ( T ),                              \
    float:      50.F,                   \
    double:     50.,                    \
    default:    50.L                    \
)
#define warning_distance_1( T )  \
warning_distance_1_base( T ) * warning_distance_1_base( T )
#define warning_distance_2_base( T )    \
_Generic                                \
(                                       \
    ( T ),                              \
    float:      40.F,                   \
    double:     40.,                    \
    default:    40.L                    \
)
#define warning_distance_2( T )  \
warning_distance_2_base( T ) * warning_distance_2_base( T )
#define warning_velocity_base( T )  \
_Generic                            \
(                                   \
    ( T ),                          \
    float:      300.F,              \
    double:     300.,               \
    default:    300.L               \
)
#define warning_velocity( T )  \
warning_velocity_base( T ) * warning_velocity_base( T )

typedef long double units;

/**
 * @brief Contains latitude, longitude, altitude and time in units
 */
struct data_input
{
    units latitude;
    units longitude;
    units altitude;
    units time;
};

/**
 * @brief Contains distance and approach velocity in units
 */
struct data_output
{
    units distance;
    units approach_velocity;
};

extern struct data_output   g_output[amount_objects_maximum];
extern char                 g_files[amount_files][size_file_name];
extern FILE *               g_data_output;
extern pthread_cond_t       g_condition;
extern pthread_mutex_t      g_mutex;
extern units                g_time;
extern size_t               g_amount_objects;
extern size_t               g_amount_observations;
extern bool                 g_write_happened;

#endif // AUXILIARY_H
