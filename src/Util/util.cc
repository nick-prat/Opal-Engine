#include "util.hh"

#include <Core/gl.hh>

#include <Util/log.hh>

void Opal::Util::PrintGLErrors()
{
    for(GLenum glErr = glGetError(); glErr != GL_NO_ERROR; glErr = glGetError())
    {
        Log::getErrorLog<SyncLogger>() << "~~  GLError (" << glErr << ") " << glErr << " ~~\n";
    }
}

std::vector<std::string> Opal::Util::splitString(const std::string &string, const char delim) {
    std::vector<std::string> strings;
    std::size_t locs = 0;
    std::size_t loce = string.find(delim);
    while(loce != std::string::npos  &&locs < string.size()) {
        strings.push_back(string.substr(locs, loce-locs));
        locs = loce + 1;
        loce = string.find(delim, locs);
    }
    if(locs < string.size()) {
        strings.push_back(string.substr(locs, string.size()));
    }
    return strings;
}

void Opal::Util::copyaiMat(const aiMatrix4x4* from, glm::mat4 &to) {
    to[0][0] = from->a1; to[1][0] = from->a2;
    to[2][0] = from->a3; to[3][0] = from->a4;
    to[0][1] = from->b1; to[1][1] = from->b2;
    to[2][1] = from->b3; to[3][1] = from->b4;
    to[0][2] = from->c1; to[1][2] = from->c2;
    to[2][2] = from->c3; to[3][2] = from->c4;
    to[0][3] = from->d1; to[1][3] = from->d2;
    to[2][3] = from->d3; to[3][3] = from->d4;
}
