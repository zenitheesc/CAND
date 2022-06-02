set(sources
    src/can.cpp
)

set(exe_sources
	${sources}
    src/cand.cpp
)

set(headers
    include/cand/can.hpp
    include/cand/cand.hpp
    include/cand/functions.hpp
)

set(test_sources
    src/receive_example.cpp
    src/transmit_example.cpp
)
