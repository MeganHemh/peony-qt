#include "directory-view-factory-manager.h"
#include "directory-view-plugin-iface.h"

#include "icon-view-factory.h"

#include <QDir>
#include <QDebug>
#include <QApplication>
#include <QPluginLoader>

#include <QSettings>

using namespace Peony;

static DirectoryViewFactoryManager *globalInstance = nullptr;

DirectoryViewFactoryManager* DirectoryViewFactoryManager::getInstance()
{
    if (!globalInstance) {
        globalInstance = new DirectoryViewFactoryManager;
    }
    return globalInstance;
}

DirectoryViewFactoryManager::DirectoryViewFactoryManager(QObject *parent) : QObject(parent)
{
    m_settings = new QSettings("UbuntuKylin Team", "Peony Qt", this);
    m_hash = new QHash<QString, DirectoryViewPluginIface*>();
    //register icon view and list view
    auto iconViewFactory = IconViewFactory::getInstance();
    registerFactory(iconViewFactory->viewIdentity(), iconViewFactory);

    //load plugins
    QDir pluginsDir(qApp->applicationDirPath());
    qDebug()<<pluginsDir;
    pluginsDir.cdUp();
    pluginsDir.cd("testdir2");
    pluginsDir.setFilter(QDir::Files);
    for (auto fileName : pluginsDir.entryList(QDir::Files)) {
        qDebug()<<fileName;
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        qDebug()<<pluginLoader.metaData();
        qDebug()<<pluginLoader.load();
        QObject *plugin = pluginLoader.instance();
        if (plugin) {
            DirectoryViewPluginIface *iface = qobject_cast<DirectoryViewPluginIface*>(plugin);
            qDebug()<<iface->name();
            qDebug()<<iface->description();
            qDebug()<<iface->viewIcon();
            qDebug()<<iface->viewIdentity();
            registerFactory(iface->viewIdentity(), iface);
        }
    }
    qDebug()<<getFactoryNames();
}

DirectoryViewFactoryManager::~DirectoryViewFactoryManager()
{

}

void DirectoryViewFactoryManager::registerFactory(const QString &name, DirectoryViewPluginIface *factory)
{
    if (m_hash->value(name)) {
        return;
    }
    m_hash->insert(name, factory);
}

QStringList DirectoryViewFactoryManager::getFactoryNames()
{
    return m_hash->keys();
}

DirectoryViewPluginIface *DirectoryViewFactoryManager::getFactory(const QString &name)
{
    return m_hash->value(name);
}

const QString DirectoryViewFactoryManager::getDefaultViewId()
{
    if (m_default_view_id_cache.isNull()) {
        auto string = m_settings->value("directory-view/default-view-id").toString();
        if (string.isEmpty()) {
            string = "Icon View";
        }
        m_default_view_id_cache = string;
    }
    return m_default_view_id_cache;
}

void DirectoryViewFactoryManager::setDefaultViewId(const QString &viewId)
{
    if (getFactoryNames().contains(viewId)) {
        m_default_view_id_cache = viewId;
    }
}

void DirectoryViewFactoryManager::saveDefaultViewOption()
{
    m_settings->setValue("directory-view/default-view-id", m_default_view_id_cache);
}
