/****************************************************************************************
 * Copyright (c) 2010 Leo Franchi <lfranchi@kde.org>                                    *
 *                                                                                      *
 * This program is free software; you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software        *
 * Foundation; either version 2 of the License, or (at your option) any later           *
 * version.                                                                             *
 *                                                                                      *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
 *                                                                                      *
 * You should have received a copy of the GNU General Public License along with         *
 * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
 ****************************************************************************************/

#include "dynamic/echonest/EchonestControl.h"

#include <echonest/Playlist.h>

#include <QComboBox>
#include <QLineEdit>


Tomahawk::EchonestControl::EchonestControl( const QString& selectedType, const QStringList& typeSelectors, QObject* parent )
    : DynamicControl ( selectedType.isEmpty() ? "Artist" : selectedType, typeSelectors, parent )
{
    setType( "echonest" );
    updateWidgets();
}

QWidget*
Tomahawk::EchonestControl::inputField()
{
    return m_input.data();
}

QWidget*
Tomahawk::EchonestControl::matchSelector()
{
    return m_match.data();
}

void 
Tomahawk::EchonestControl::setSelectedType ( const QString& type )
{
    if( !m_input.isNull() )
        delete m_input.data();
    if( !m_match.isNull() )
        delete m_match.data();
    
    Tomahawk::DynamicControl::setSelectedType ( type );
    updateWidgets();
}

Echonest::DynamicPlaylist::PlaylistParamData
Tomahawk::EchonestControl::toENParam() const
{
    return m_data;
}

QString Tomahawk::EchonestControl::input() const
{
    return m_data.second.toString();
}

QString Tomahawk::EchonestControl::match() const
{
    return m_matchString;
}

void Tomahawk::EchonestControl::setInput(const QString& input)
{
    // TODO generate widgets
    m_data.second = input;
    updateWidgetsFromData();
}

void Tomahawk::EchonestControl::setMatch(const QString& match)
{
    // TODO generate widgets
    m_matchString = match;
    updateWidgetsFromData();
}


void 
Tomahawk::EchonestControl::updateWidgets()
{
    if( !m_input.isNull() )
        delete m_input.data();
    if( !m_match.isNull() )
        delete m_match.data();
    
    // make sure the widgets are the proper kind for the selected type, and hook up to their slots
    if( selectedType() == "Artist" ) {
        m_currentType = Echonest::DynamicPlaylist::Artist;
        
        QComboBox* match = new QComboBox();
        QLineEdit* input =  new QLineEdit();
        
        match->addItem( "Limit To", Echonest::DynamicPlaylist::ArtistType );
        match->addItem( "Similar To", Echonest::DynamicPlaylist::ArtistRadioType );
        m_matchString = match->itemText( 0 );
        
        input->setPlaceholderText( "Artist name" );
        input->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Fixed );
        
        connect( match, SIGNAL( currentIndexChanged(int) ), this, SLOT( updateData() ) );
        connect( match, SIGNAL( currentIndexChanged(int) ), this, SIGNAL( changed() ) );
        connect( input, SIGNAL( textChanged(QString) ), this, SLOT( updateData() ) );
        connect( input, SIGNAL( editingFinished() ), this, SIGNAL( changed() ) );
        
        match->hide();
        input->hide();
        m_match = QWeakPointer< QWidget >( match );
        m_input = QWeakPointer< QWidget >( input );
    } else {
        m_match = QWeakPointer<QWidget>( new QWidget );
        m_input = QWeakPointer<QWidget>( new QWidget );
    }
}

void 
Tomahawk::EchonestControl::updateData()
{
    if( selectedType() == "Artist" ) {
        QComboBox* combo = qobject_cast<QComboBox*>( m_match.data() );
        if( combo ) {
            m_matchString = combo->itemData( combo->currentIndex() ).toString();
        }
        QLineEdit* edit = qobject_cast<QLineEdit*>( m_input.data() );
        if( edit && !edit->text().isEmpty() ) {
            m_data.first = m_currentType;
            m_data.second = edit->text();
        }
    }
}

// fills in the current widget with the data from json or dbcmd (m_data.second and m_matchString)
void Tomahawk::EchonestControl::updateWidgetsFromData()
{
    if( selectedType() == "Artist" ) {
        QComboBox* combo = qobject_cast<QComboBox*>( m_match.data() );
        if( combo )
            combo->setCurrentIndex( combo->findData( m_matchString ) );
        QLineEdit* edit = qobject_cast<QLineEdit*>( m_input.data() );
        if( edit )
            edit->setText( m_data.second.toString() );
    }
}

