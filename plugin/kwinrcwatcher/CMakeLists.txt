set(PLUGIN_NAME "kwinrcwatcherplugin")
set(LIB_NAME "kwinrcwatcher")

add_library(${PLUGIN_NAME} SHARED ${PLUGIN_NAME}.cpp ${LIB_NAME}.cpp)
find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS DBus Qml)

target_link_libraries(${PLUGIN_NAME}
    Qt6::Qml
    Qt6::DBus
)

install(TARGETS ${PLUGIN_NAME} DESTINATION ${KDE_INSTALL_QMLDIR}/com/github/luisbocanegra/${LIB_NAME})
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/com/github/luisbocanegra/${LIB_NAME})
