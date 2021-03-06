#ifndef TOPLEVELITEM_HPP
#define TOPLEVELITEM_HPP

#include <QQuickItem>
#include <QScopedPointer>
#include "textureitem.hpp"
#include "utils.hpp"

class TopLevelContainer;
class TopLevelShadow;
class QPropertyAnimation;

class TopLevelItem : public TextureItem {
	Q_OBJECT
	Q_PROPERTY(qreal shade READ shade WRITE setShade NOTIFY shadeChanged FINAL)
	Q_PROPERTY(qreal boundary READ boundary WRITE setBoundary NOTIFY boundaryChanged FINAL)
	Q_PROPERTY(TopLevelContainer *container READ container CONSTANT FINAL)
	Q_PROPERTY(TopLevelShadow *shadow READ shadow CONSTANT FINAL)
	Q_PROPERTY(bool autohide READ autohide WRITE setAutohide NOTIFY autohideChanged FINAL)
public:
	TopLevelItem(QQuickItem *parent = 0);
	~TopLevelItem();
	qreal shade() const;
	void setShade(qreal shade);
	qreal boundary() const;
	void setBoundary(qreal boundary);
	TopLevelContainer *container() const;
	TopLevelShadow *shadow() const;
	bool autohide() const;
	void setAutohide(bool autohide);
public slots:
	void show();
	void hide();
signals:
	void shadeChanged();
	void containerPositionChanged();
	void boundaryChanged();
	void autohideChanged();
	void canceled();
private slots:
	void updateParentItem();
	void updateFocusState();
	void updateContainerRect();
	void handleAnimationFinished();
private:
	void componentComplete();
	void geometryChanged(const QRectF &new_, const QRectF &old);
	QSGGeometry *createSGGeometry();
	void updateSGGeometry(QSGGeometry *geometry);
	QByteArray fragmentShader() const;
	QByteArray vertexShader() const;
	void bind(QOpenGLShaderProgram *prog, const RenderState &state);
	void link(QOpenGLShaderProgram *prog);
	void initializeGL();
	void finalizeGL();
	bool prepareToRender();

	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	struct Data;
	Data *d;
};

#endif // TOPLEVELITEM_HPP
