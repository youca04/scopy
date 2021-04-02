cmake_minimum_required(VERSION 3.9.6)

add_scopy_plugin(
    demo
    QT_DEPENDS
    Concurrent
    SCOPY_DEPENDS
    scopy::core
    scopy::gui
    project_options
    project_warnings
    SOURCES
    demo_plugin.cpp
    demo_tool.cpp
    demo_data_source.cpp
)
