set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS 
    Widgets
    Svg
    Charts
    REQUIRED)

set(GUI_SOURCES
    view/ScoreListoView.cpp
    view/HomeWidget/HomeWidget.cpp
    view/HomeWidget/FileSelectorWidget/FileSelectorWidget.cpp
    view/HomeWidget/RecorderWidget/RecorderWidget.cpp
    view/HomeWidget/StreamSelectorWidget/StreamSelectorWidget.cpp
    view/AdvancedConverterWidget/AdvancedConverterWidget.cpp
    view/common/ParametersWidget/RhythmParametersWidget.cpp
    view/common/ParametersWidget/StepParametersWidget.cpp
    view/common/ParametersWidget/AudioReaderParametersWidget.cpp
    view/common/ParametersWidget/McLeodParametersWidget.cpp
    view/common/ParametersWidget/TimeRangeParametersWidget.cpp
    view/common/AudioPlayerWidget/AudioPlayerWidget.cpp
    view/common/PlotWidget/PlotWidget.cpp
    view/common/ScorePlotWidget/ScorePlotWidget.cpp
    view/common/ProgressBarWidget/ProgressBarWidget.cpp
    model/ScoreListoModel.cpp
    controller/ScoreListoController.cpp
    main.cpp
)

include_directories(view)
include_directories(view/HomeWidget)
include_directories(view/HomeWidget/FileSelectorWidget)
include_directories(view/HomeWidget/RecorderWidget)
include_directories(view/HomeWidget/StreamSelectorWidget)
include_directories(view/AdvancedConverterWidget)
include_directories(view/common/ParametersWidget)
include_directories(view/common/AudioPlayerWidget)
include_directories(view/common/PlotWidget)
include_directories(view/common/ScorePlotWidget)
include_directories(view/common/ProgressBarWidget)
include_directories(model)
include_directories(controller)

add_executable(scorelisto_gui ${GUI_SOURCES})
target_link_libraries(scorelisto_gui scorelisto)
target_link_libraries(scorelisto_gui Qt5::Widgets)
target_link_libraries(scorelisto_gui Qt5::Svg)
target_link_libraries(scorelisto_gui Qt5::Charts)
# target_link_libraries(scorelisto_gui Qt5::Font)
# target_link_libraries(scorelisto_gui Qt5::FileDialog)
# target_link_libraries(scorelisto_gui Qt5::File)
# target_link_libraries(scorelisto_gui Qt5::TextStream)
# target_link_libraries(scorelisto_gui Qt5::MessageBox)
# target_link_libraries(scorelisto_gui Qt5::PrintDialog)
# target_link_libraries(scorelisto_gui Qt5::Printer)

# configure_file(resources ${EXECUTABLE_OUTPUT_PATH}/resources COPYONLY)

add_custom_command( TARGET scorelisto_gui POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_CURRENT_LIST_DIR}/resources $<TARGET_FILE_DIR:scorelisto_gui>/resources)