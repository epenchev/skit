/*
 * IDGenerator.h
 *
 * Copyright (C) 2013  Emil Penchev, Bulgaria
 * Created on: Oct 25, 2013
 *      Author: emo
 */

#ifndef IDGENERATOR_H_
#define IDGENERATOR_H_

class IDGenerator
{
public:
    static IDGenerator& Instance()
    {
        static IDGenerator s_generator;
        return s_generator;
    }

    unsigned Next() { return ++m_id; }

private:
    IDGenerator() : m_id(0) {}
    unsigned m_id;
};

#endif /* IDGENERATOR_H_ */
