/*
 * ErrorCode.cpp
 *
 * Copyright (C) 2013  Emil Penchev, Bulgaria
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 *  Created on: Feb 12, 2014
 *      Author: emo
 */

#include "utils/ErrorCode.h"

ErrorCode::ErrorCode() : m_iset(false)
{}

ErrorCode::ErrorCode(const ErrorCode& err)
{
	m_iset = false;
	SetMessage(err.Message());
}

ErrorCode::~ErrorCode()
{}

const char* ErrorCode::Message() const
{
	return m_message.c_str();
}

void ErrorCode::SetMessage(const std::string& message)
{
	if (!message.empty())
	{
		m_iset = true;
		m_message = message;
	}
}

void ErrorCode::operator = (bool iset)
{
	m_iset = iset;
	if (!m_iset)
	{
		m_message.clear();
	}
}

ErrorCode& ErrorCode::operator = (const ErrorCode& err)
{
	m_iset = false;
	SetMessage(err.Message());

	return *this;
}

ErrorCode::operator bool() const
{
	return m_iset;
}

bool ErrorCode::operator!() const
{
	return !(m_iset);
}


