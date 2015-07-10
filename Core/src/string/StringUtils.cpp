#include <string/StringUtils.hpp>

#include <cstring>
#include <cstdarg>
#include <memory>

namespace Ra
{
    namespace StringUtils
    {
        std::string getFileExt(const std::string& str)
        {
            std::string res;
            unsigned pos = str.find_last_of('.');
            res = pos < str.size() ? str.substr(pos) : "";
            return res;
        }


        std::string getDirName(const std::string& path)
        {
            // We remove any trailing slashes.
            unsigned int pos = path.find_last_not_of('/');

            // Don't strip the last / from "/"
            if (pos == std::string::npos)
            {
                pos = path.find_first_of("/");
            }

            std::string res;
            res = path.substr(0, pos + 1);

            // Now find the previous slash and cut the string.
            pos = res.find_last_of('/');

            // The directory is actually "/" because the last slash is in first position.
            // In that case we should return "/"
            if (pos == 0)
            {
                res = "/";
            }
            else if (pos != std::string::npos)
            {
                res = res.substr(0, pos);
            }
            else
            {
                res = ".";
            }

            return res;
        }

        std::string getBaseName(const std::string& path)
        {
            std::string res;
            // We remove any trailing slashes.
            unsigned int pos = path.find_last_not_of('/');
            // Don't strip the last / from "/"
            if (pos == std::string::npos)
            {
                pos = path.find_first_of("/");
            }

            res = path.substr(0, pos + 1);

            // Now find the previous slash and cut the string.
            pos = res.find_last_of('/');

            if (pos != std::string::npos)
            {
                res = res.substr(pos + 1);
            }

            return res;
        }


        int stringPrintf(std::string& str, const char* fmt, ...)
        {
            // Random guessing value from the size of the format string.
            int size = strlen(fmt) * 2;
            int finalSize = 0;
            str.clear();

            std::unique_ptr<char[]> buffer;
            va_list args;

            while (1)
            {
                // Dynamically allocate a string and assign it to the unique ptr
                buffer.reset(new char[size]);

                // Attempt to printf into the buffer
                va_start(args, fmt);
                finalSize = vsnprintf(&buffer[0], size, fmt, args);
                va_end(args);

                // If our buffer was too small, we know that final_size
                // gives us the required buffer size.
                if (finalSize >= size)
                {
                    size = std::max(size + 1, finalSize);
                }
                else
                {
                    break;
                }
            }
            if (finalSize > 0)
            {
                str = std::string(buffer.get());
            }
            return finalSize;
        }

        int appendPrintf(std::string& str, const char* fmt, ...)
        {
            std::string toAppend;
            va_list args;
            va_start(args, fmt);
            int result = stringPrintf(toAppend, fmt, args);
            str += toAppend;
            va_end(args);
            return result;
        }

    }
}
