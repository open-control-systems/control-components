idf_component_register(
    SRCS
    "test_analog_config.cpp"
    "test_analog_config_store.cpp"
    "test_analog_sample_reader.cpp"

    "ds18b20/test_store.cpp"
    "ds18b20/test_parse_configuration.cpp"

    "ldr/test_analog_sensor.cpp"
    "soil/test_analog_sensor.cpp"

    REQUIRES
    "unity"
    "ocs_test"
    "ocs_sensor"
)
