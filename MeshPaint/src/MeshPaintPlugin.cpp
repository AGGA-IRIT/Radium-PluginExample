#include <MeshPaintPlugin.hpp>

#include <QAction>
#include <QIcon>
#include <QToolBar>

#include <Engine/Scene/Entity.hpp>
#include <Engine/Scene/EntityManager.hpp>
#include <Engine/Scene/SignalManager.hpp>
#include <Engine/Scene/SystemDisplay.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Rendering/RenderObject.hpp>
#include <Engine/Rendering/RenderObjectManager.hpp>

#include <Gui/SelectionManager/SelectionManager.hpp>
#include <Gui/Utils/PickingManager.hpp>

#include "ui_MeshPaintUI.h"
#include <MeshPaintSystem.hpp>
#include <UI/MeshPaintUI.hpp>

namespace MeshPaintPlugin {

MeshPaintPluginC::MeshPaintPluginC() :
    m_selectionManager( nullptr ),
    m_PickingManager( nullptr ),
    m_system( nullptr ),
    m_paintColor( Ra::Core::Utils::Color::Red() ),
    m_isPainting( false ) {
    m_widget = new MeshPaintUI();
    QColor color( int( m_paintColor( 0 ) * Scalar( 255 ) ),
                  int( m_paintColor( 1 ) * Scalar( 255 ) ),
                  int( m_paintColor( 2 ) * Scalar( 255 ) ),
                  int( m_paintColor( 3 ) * Scalar( 255 ) ) );
    m_widget->ui->changeColor_pb->setPalette( QPalette( color ) );
}

MeshPaintPluginC::~MeshPaintPluginC() {}

void MeshPaintPluginC::registerPlugin( const Ra::Plugins::Context& context ) {
    // register selection context
    m_selectionManager = context.m_selectionManager;
    m_PickingManager   = context.m_pickingManager;
    m_system           = new MeshPaintSystem;
    context.m_engine->registerSystem( "MeshPaintSystem", m_system );
    connect( m_selectionManager,
             &Ra::Gui::SelectionManager::currentChanged,
             this,
             &MeshPaintPluginC::onCurrentChanged );
    connect( m_widget, &MeshPaintUI::paintColor, this, &MeshPaintPluginC::activePaintColor );
    connect( m_widget, &MeshPaintUI::colorChanged, this, &MeshPaintPluginC::changePaintColor );
    connect( m_widget, &MeshPaintUI::bakeToDiffuse, this, &MeshPaintPluginC::bakeToDiffuse );
    connect( this, &MeshPaintPluginC::askForUpdate, &context, &Ra::Plugins::Context::askForUpdate );
}

bool MeshPaintPluginC::doAddWidget( QString& name ) {
    name = "MeshPaint";
    return true;
}

QWidget* MeshPaintPluginC::getWidget() {
    return m_widget;
}

bool MeshPaintPluginC::doAddMenu() {
    return false;
}

QMenu* MeshPaintPluginC::getMenu() {
    return nullptr;
}

bool MeshPaintPluginC::doAddAction( int& nb ) {
    nb = 0;
    return false;
}

QAction* MeshPaintPluginC::getAction( int /*id*/ ) {
    return nullptr;
}

void MeshPaintPluginC::activePaintColor( bool on ) {
    m_isPainting = on;
    m_system->startPaintMesh( on );
    askForUpdate();
}

void MeshPaintPluginC::changePaintColor( const QColor& color ) {
    m_paintColor = Ra::Core::Utils::Color( Scalar( color.red() ) / 255,
                                           Scalar( color.green() ) / 255,
                                           Scalar( color.blue() ) / 255,
                                           Scalar( 1.0 ) );
}

void MeshPaintPluginC::bakeToDiffuse() {
    if ( m_isPainting &&
         Ra::Core::Utils::Index::Invalid() != m_selectionManager->currentItem().m_roIndex )
    { m_system->bakeToDiffuse(); }
    askForUpdate();
}

void MeshPaintPluginC::onCurrentChanged( const QModelIndex& /*current*/,
                                         const QModelIndex& /*prev*/ ) {
    if ( m_isPainting &&
         Ra::Core::Utils::Index::Invalid() != m_selectionManager->currentItem().m_roIndex )
    { m_system->paintMesh( m_PickingManager->getCurrent(), m_paintColor ); }
    askForUpdate();
}

} // namespace MeshPaintPlugin
