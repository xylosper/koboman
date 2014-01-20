#include "actionlistitem.hpp"
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlComponent>

static QMetaMethod mmPolishAndUpdate;
static QMetaMethod mmTextChanged;

struct ActionListItem::Data {
	QQmlComponent *component = nullptr;
	QList<QQuickItem*> textItems;
	QList<Action*> actions;
	QStringList texts;
	QFont font;
	Qt::AlignmentFlag valign = Qt::AlignVCenter, haligh = Qt::AlignLeft;
	qreal textHeight = -1.0;
	bool interactive = false;
	template<typename T>
	void setTextProperty(const char *name, const T &t) {
		for (auto item : textItems)
			item->setProperty(name, t);
	}
};

ActionListItem::ActionListItem(QQuickItem *parent)
: ItemListItem(parent), d(new Data) {
	d->font = Theme::font();
	d->textHeight = Theme::lineHeight() - 2*Theme::padding();
	setVerticalPadding(Theme::padding());
	setHorizontalPadding(Theme::padding());
	connect(this, &ItemListItem::listChanged, this, &ActionListItem::itemsChanged);
}

ActionListItem::~ActionListItem() {
	qDeleteAll(d->textItems);
	delete d->component;
	delete d;
}

QStringList ActionListItem::texts() const {
	return d->texts;
}

QByteArray ActionListItem::sourceCode() const {
	return R"(
		import QtQuick 2.2
		import net.xylosper.Mobile 1.0
		Item {
			property int orientation: Qt.Horizontal
			property alias iconSize: icon.height
			property alias text: text.text
			Image {
				id: icon
				objectName: "icon"
				width: height
			}
			Text {
				id: text
				objectName: "text"
			}
		}
	)";
}

QQmlListProperty<Action> ActionListItem::actions() const {
	static auto append = [] (QQmlListProperty<Action> *list, Action *action) -> void {
		static_cast<ActionListItem*>(list->object)->append(action);
	};
	static auto count = [] (QQmlListProperty<Action> *list) -> int {
		return static_cast<ActionListItem*>(list->object)->d->actions.size();
	};
	static auto at = [] (QQmlListProperty<Action> *list, int index) -> Action* {
		return static_cast<ActionListItem*>(list->object)->d->actions.at(index);
	};
	static auto clear = [] (QQmlListProperty<Action> *list) -> void {
		static_cast<ActionListItem*>(list->object)->d->actions.clear();
	};
	return QQmlListProperty<Action>(const_cast<ActionListItem*>(this), nullptr, append, count, at, clear);
}

void ActionListItem::append(Action *action) {
	if (!d->component) {
		d->component = new QQmlComponent(QQmlEngine::contextForObject(this)->engine());
		d->component->setData(sourceCode(), QUrl());
	}
	clear();
	if (d->textItems.size() < texts.size()) {
		d->textItems.reserve(texts.size());
		while (d->textItems.size() < texts.size()) {
			auto item = static_cast<QQuickItem*>(d->component->create());
			item->setProperty("verticalAlignment", d->valign);
			item->setProperty("horizontalAlignment", d->haligh);
			item->setProperty("font", d->font);
			auto attached = this->attached(item, true);
			attached->setInteractive(true);
			attached->setThickness(d->textHeight);
			if (!mmPolishAndUpdate.isValid()) {
				auto find = [] (QObject *object, const char *name) {
					auto *mm = object->metaObject();
					const int size = mm->methodCount();
					for (int i=0; i<size; ++i) {
						if (!qstrcmp(mm->method(i).name(), name))
							return mm->method(i);
					}
					return QMetaMethod();
				};
				mmPolishAndUpdate = find(this, "polishAndUpdate");
				mmTextChanged = find(item, "textChanged");
			}
			connect(item, mmTextChanged, this, mmPolishAndUpdate);
			d->textItems.append(item);
		}
	}
	for (int i=0; i<texts.size(); ++i) {
		d->textItems[i]->setProperty("text", texts[i]);
		ItemListItem::append(d->textItems[i]);
	}
	emit textsChanged();
}

void ActionListItem::setTexts(const QStringList &texts) {
	d->texts = texts;
	if (!d->component) {
		d->component = new QQmlComponent(QQmlEngine::contextForObject(this)->engine());
		d->component->setData(sourceCode(), QUrl());
	}
	clear();
	if (d->textItems.size() < texts.size()) {
		d->textItems.reserve(texts.size());
		while (d->textItems.size() < texts.size()) {
			auto item = static_cast<QQuickItem*>(d->component->create());
			item->setProperty("verticalAlignment", d->valign);
			item->setProperty("horizontalAlignment", d->haligh);
			item->setProperty("font", d->font);
			auto attached = this->attached(item, true);
			attached->setInteractive(true);
			attached->setThickness(d->textHeight);
			if (!mmPolishAndUpdate.isValid()) {
				auto find = [] (QObject *object, const char *name) {
					auto *mm = object->metaObject();
					const int size = mm->methodCount();
					for (int i=0; i<size; ++i) {
						if (!qstrcmp(mm->method(i).name(), name))
							return mm->method(i);
					}
					return QMetaMethod();
				};
				mmPolishAndUpdate = find(this, "polishAndUpdate");
				mmTextChanged = find(item, "textChanged");
			}
			connect(item, mmTextChanged, this, mmPolishAndUpdate);
			d->textItems.append(item);
		}
	}
	for (int i=0; i<texts.size(); ++i) {
		d->textItems[i]->setProperty("text", texts[i]);
		ItemListItem::append(d->textItems[i]);
	}
	emit textsChanged();
}

QFont ActionListItem::font() const {
	return d->font;
}

void ActionListItem::setFont(const QFont &font) {
	if (_Change(d->font, font)) {
		d->setTextProperty("font", d->font);
		polishAndUpdate();
		emit fontChanged();
	}
}

Qt::AlignmentFlag ActionListItem::verticalAlignment() const {
	return d->valign;
}

Qt::AlignmentFlag ActionListItem::horizontalAlignment() const {
	return d->haligh;
}

void ActionListItem::setVerticalAlignment(Qt::AlignmentFlag alignment) {
	if (_Change(d->valign, alignment)) {
		d->setTextProperty("verticalAlignment", alignment);
		emit verticalAlignmentChanged();
	}
}

void ActionListItem::setHorizontalAlignment(Qt::AlignmentFlag alignment) {
	if (_Change(d->haligh, alignment)) {
		d->setTextProperty("horizontalAlignment", alignment);
		emit horizontalAlignmentChanged();
	}
}

bool ActionListItem::isInteractive() const {
	return d->interactive;
}

void ActionListItem::setInteractive(bool interactive) {
	if (_Change(d->interactive, interactive)) {
		for (auto item : d->textItems)
			attached(item, false)->setInteractive(interactive);
		emit interactiveChanged();
	}
}

void ActionListItem::setText(int index, const QString &text) {
	if (auto item = d->textItems.value(index))
		item->setProperty("text", text);
}

qreal ActionListItem::textHeight() const {
	return d->textHeight;
}

void ActionListItem::setTextHeight(qreal height) {
	if (_Change(d->textHeight, height)) {
		for (auto item : d->textItems)
			attached(item, false)->setThickness(d->textHeight);
		emit textHeightChanged();
	}
}
