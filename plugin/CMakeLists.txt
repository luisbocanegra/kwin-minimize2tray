set(PLUGIN_NAME "trayiconplugin")
set(LIB_NAME "trayicon")

add_library(${PLUGIN_NAME} SHARED ${PLUGIN_NAME}.cpp ${LIB_NAME}.cpp)
find_package(Qt6 REQUIRED COMPONENTS Widgets DBus Qml)
find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS StatusNotifierItem Service)

target_link_libraries(${PLUGIN_NAME}
    Qt6::Widgets
    Qt6::Qml
    Qt6::DBus
    KF6::StatusNotifierItem
    KF6::Service
)

install(TARGETS ${PLUGIN_NAME} DESTINATION ${KDE_INSTALL_QMLDIR}/com/github/luisbocanegra/${LIB_NAME})
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/com/github/luisbocanegra/${LIB_NAME})
