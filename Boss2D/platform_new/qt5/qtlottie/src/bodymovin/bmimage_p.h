/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the lottie-qt module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef BMIMAGE_P_H
#define BMIMAGE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QImage>
#include <QPointF>

#include "bmproperty_p.h"
#include "bmspatialproperty_p.h"

class QJsonObject;

class BMImage : public BMBase
{
public:
    BMImage() = default;
    explicit BMImage(const BMImage &other);
    BMImage(const QJsonObject &definition, BMBase *parent = nullptr);

    BMBase *clone() const override;

    void construct(const QJsonObject &definition);

    void updateProperties(int frame) override;
    void render(LottieRenderer &renderer) const override;

    QPointF position() const;
    qreal radius() const;

    QPointF getCenter() const { return m_center; }
    QImage getImage() const { return m_image; }

protected:
    BMSpatialProperty m_position;
    BMProperty<qreal> m_radius;

    QImage m_image;
    QPointF m_center;
};

#endif // BMIMAGE_P_H
