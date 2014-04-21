#include "opengldrawitem.hpp"

OpenGLDrawItem::OpenGLDrawItem(QQuickItem *parent)
: GeometryItem(parent) {
    setFlag(ItemHasContents, true);
    connect(this, &QQuickItem::windowChanged, [this] (QQuickWindow *window) {
        m_win = window;
        if (window) {
            connect(window, &QQuickWindow::sceneGraphInitialized, this, &OpenGLDrawItem::tryInitGL, Qt::DirectConnection);
            connect(window, &QQuickWindow::beforeRendering, this, &OpenGLDrawItem::tryInitGL, Qt::DirectConnection);
            connect(window, &QQuickWindow::sceneGraphInvalidated, this, &OpenGLDrawItem::finalizeGL, Qt::DirectConnection);
        }
    });
}

OpenGLDrawItem::~OpenGLDrawItem() {

}

QSGNode *OpenGLDrawItem::updatePaintNode(QSGNode *old, UpdatePaintNodeData *data) {
    tryInitGL();
    Q_UNUSED(data);
    m_node = static_cast<QSGGeometryNode*>(old);
    if (!m_node) {
        m_node = new QSGGeometryNode;
        m_node->setGeometry(createGeometry());
        m_node->setMaterial(createMaterial());
        m_node->setFlags(QSGNode::OwnsGeometry | QSGNode::OwnsMaterial);
        m_node->geometry()->setDrawingMode(drawingMode());
        reserve(UpdateAll, false);
    }
    if (isReserved(UpdateMaterial)) {
        auto m = m_node->material();
        if (_Change(m, updateMaterial(m)))
            m_node->setMaterial(m);
        m_node->markDirty(QSGNode::DirtyMaterial);
    }
    if (isReserved(UpdateGeometry)) {
        auto g = m_node->geometry();
        if (_Change(g, updateGeometry(g)))
            m_node->setGeometry(g);
        m_node->markDirty(QSGNode::DirtyGeometry);
    }
    m_updates = 0;
    afterUpdate();
    return m_node;
}
