# include path for Aggregation
INCLUDEPATH *= $${PWD}
CONFIG( debug, debug|release ) {
    unix:LIBS    *= -lAggregation_debug
    win32:LIBS   *= -lAggregation_d
} else {
    LIBS  *= -lAggregation
}
