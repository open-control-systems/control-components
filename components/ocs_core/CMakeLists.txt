idf_component_register(
    SRCS
    "basic_static_mutex.cpp"
    "static_mutex.cpp"
    "static_mutex_isr.cpp"
    "basic_static_recursive_mutex.cpp"
    "static_recursive_mutex.cpp"
    "static_event_group.cpp"
    "cond.cpp"
    "lock_guard.cpp"
    "version.cpp"
    "version_to_str.cpp"
    "future.cpp"
    "rate_limiter.cpp"
    "operation_guard.cpp"
    "file_stream_reader.cpp"
    "stream_transceiver.cpp"

    REQUIRES
    "json"
    "freertos"

    INCLUDE_DIRS
    ".."
)
