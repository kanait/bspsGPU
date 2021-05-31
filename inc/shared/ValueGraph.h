#ifndef __VALUE_GRAPH_H__
#define __VALUE_GRAPH_H__

#if defined(MACOS)
#  include <OpenGL/gl.h>
#else
#  include <GL/gl.h>
#endif

#include <string>
#include <vector>

class CValueGraph
{
public:
    CValueGraph();
    virtual ~CValueGraph();

    void SetOrigin(GLuint iX, GLuint iY) 
        { m_iOriginX = iX; m_iOriginY = iY; };
    void SetWidth(GLuint iWidth) 
    { 
        m_iWidth = iWidth; 
        m_vValues.resize(GetDiagramWidth());
        memset(&m_vValues[0], 0, sizeof(GLuint) * m_vValues.size());
        m_iValueIdx = 0;
    }
    void SetHeight(GLuint iHeight)
    { m_iHeight = iHeight; }
    void SetDescription(const char *pszDescription) 
        { m_strDescription = pszDescription; };
    void SetUnitName(const char *pszUnit) 
        { m_strUnitName = pszUnit; };
    void SetMaxValue(GLuint iMaxVal) 
        { m_iMaxValue = iMaxVal; };
    void PushValue(GLuint iValue)
    {
        if (m_iValueIdx == m_vValues.size())
            m_iValueIdx = 0;
        m_vValues[m_iValueIdx] = iValue;
        m_iValueIdx++;
        if (iValue > m_iMaxValue)
            m_iMaxValue = iValue;
    }
    bool Render();

protected:
    GLuint m_iWidth;
    GLuint m_iHeight;
    GLuint m_iOriginX;
    GLuint m_iOriginY;
    std::string m_strDescription;
    std::string m_strUnitName;
    GLuint m_iMaxValue;
    std::vector<GLuint> m_vValues;
    GLuint m_iValueIdx;

    GLuint GetDiagramWidth() { return m_iWidth - 10 - 19; };

    #define LINE_BUFFER_MAX_VERT 4096
private:
    void beginWinCoords();
    void endWinCoords();
    void glPrint(int x, int y, const char *s, void *font);
};

#endif
