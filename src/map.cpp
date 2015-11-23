#include <QFileInfo>
#include <QDir>
#include "downloader.h"
#include "ll.h"
#include "map.h"

#include <QDebug>


Map::Map(const QString &name, const QString &url)
{
	_name = name;
	_url = url;

	connect(&Downloader::instance(), SIGNAL(finished()), this,
	  SLOT(emitLoaded()));

	QDir::home().mkpath(QString(TILES_DIR"/%1").arg(_name));
}

void Map::emitLoaded()
{
	emit loaded();
}

void Map::loadTiles(QList<Tile> &list)
{
	QList<Download> dl;

	for (int i = 0; i < list.size(); ++i) {
		Tile &t = list[i];
		QString file = QString("%1/"TILES_DIR"/%2/%3-%4-%5.png")
		  .arg(QDir::homePath()).arg(_name).arg(t.zoom()).arg(t.xy().rx())
		  .arg(t.xy().ry());
		QFileInfo fi(file);

		if (fi.exists())
			t.pixmap().load(file);
		else {
			t.pixmap() = QPixmap(TILE_SIZE, TILE_SIZE);
			t.pixmap().fill();

			QString url(_url);
			url.replace("$z", QString::number(t.zoom()));
			url.replace("$x", QString::number(t.xy().x()));
			url.replace("$y", QString::number(t.xy().y()));
			dl.append(Download(url, file));
		}
	}

	if (!dl.empty())
		Downloader::instance().get(dl);
}
