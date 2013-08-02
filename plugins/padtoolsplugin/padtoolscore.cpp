/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2013 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *  Main developer: Eric MAEKER, <eric.maeker@gmail.com>                   *
 *  Contributors:                                                          *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
/*!
 * \class PadTools::Internal::PadToolsCore
 * Central place for the PadTools plugin.
 */

#include "padtoolscore.h"
#include "padtoolsimpl.h"
#include "tokenmodel.h"
#include "padtoolscontextualwidgetmanager.h"

#include <coreplugin/icore.h>

#include <utils/log.h>
#include <utils/global.h>
#include <translationutils/constants.h>

#include <QDebug>

enum {CreateTestingTokens=false};

using namespace PadTools;
using namespace Internal;
using namespace Trans::ConstantTranslations;

namespace {
class TestingToken : public Core::IToken
{
public:
    TestingToken(const QString &name, const QVariant &value) :
        IToken(name),
        _value(value)
    {
    }

    QVariant testValue() const {return "TEST";}
    QVariant value() const {return _value;}

private:
     QVariant _value;
};
}

namespace PadTools {
namespace Internal {
class PadToolsCorePrivate
{
public:
    PadToolsCorePrivate(PadToolsCore *parent) :
        _impl(0),
        _widgetManager(0),
        _tokenModel(0),
        q(parent)
    {
    }

    ~PadToolsCorePrivate()
    {
    }

    void registerTestingTokens()
    {
        // Register testing tokens (A, B, C, D)
        Core::IToken *t;
        QVector<Core::IToken *> _tokens;

        t = new TestingToken("test.A", "Token A");
        t->setSortIndex(0);
        t->setUntranslatedHumanReadableName("TokenA");
        _tokens << t;

        t = new TestingToken("test.B", "Token B");
        t->setSortIndex(1);
        t->setUntranslatedHumanReadableName("TokenB");
        _tokens << t;

        t = new TestingToken("test.C", "Token C");
        t->setSortIndex(2);
        t->setUntranslatedHumanReadableName("TokenC");
        _tokens << t;

        t = new TestingToken("test.D", "Token D");
        t->setSortIndex(3);
        t->setUntranslatedHumanReadableName("TokenD");
        _tokens << t;

        if (_impl->tokenPool()) {
            LOG_FOR(q, "Registering  testing tokens");
            _impl->tokenPool()->addTokens(_tokens);
        } else {
            LOG_FOR(q, "PadTools object is not available, can not register the testing tokens");
        }
    }

public:
    PadToolsImpl* _impl;
    PadToolsContextualWidgetManager *_widgetManager;
    TokenModel *_tokenModel;

private:
    PadToolsCore *q;
};
} // namespace Internal
} // end namespace PadTools

// Singleton initialization
PadToolsCore *PadToolsCore::_instance = 0;

/**
 * Singleton access. This object creates its instance in the Ctor. So you should never
 * request the ctor more than once.
 */
PadTools::Internal::PadToolsCore &PadTools::Internal::PadToolsCore::instance() // static
{
    Q_ASSERT(_instance);
    return *_instance;
}

/*! Constructor of the PadTools::Internal::PadToolsCore class */
PadToolsCore::PadToolsCore(QObject *parent) :
    QObject(parent),
    d(new PadToolsCorePrivate(this))
{
    _instance = this;
    setObjectName("PadToolsCore");
    d->_impl = new PadToolsImpl(this);
    Core::ICore::instance()->setPadTools(d->_impl);
}

/*! Destructor of the PadTools::Internal::PadToolsCore class */
PadToolsCore::~PadToolsCore()
{
    _instance = 0;
    if (d)
        delete d;
    d = 0;
}

/*! Initializes the object with the default values. Return true if initialization was completed. */
bool PadToolsCore::initialize()
{
    d->_widgetManager = new PadToolsContextualWidgetManager(this);
    d->_tokenModel = new TokenModel(this);

    if (!Utils::isReleaseCompilation() && CreateTestingTokens)
        d->registerTestingTokens();

    connect(Core::ICore::instance(), SIGNAL(coreOpened()), this, SLOT(postCoreInitalization()), Qt::UniqueConnection);
    return true;
}

/** Post core initialization */
void PadToolsCore::postCoreInitalization()
{
    d->_tokenModel->initialize();
}

QAbstractItemModel *PadToolsCore::tokenModel() const
{
    return d->_tokenModel;
}

Core::ITokenPool *PadToolsCore::tokenPool() const
{
    return d->_impl->tokenPool();
}
