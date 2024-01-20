#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

struct Point
{
    int number;
    double x, y, z;
};

int sign(const Point& a, const Point& b, const Point& c)
{
    return ((a.x - c.x) * (b.x - c.x) + (a.y - c.y) * (b.y - c.y) + (a.z - c.z) * (b.z - c.z));
}

bool isPointInTriangle(const Point& p, const Point& a, const Point& b, const Point& c)
{
    int pointRelativeToEdges1 = sign(p, a, b);
    int pointRelativeToEdges2 = sign(p, b, c);
    int pointRelativeToEdges3 = sign(p, c, a);
    return ((pointRelativeToEdges1 >= 0) && (pointRelativeToEdges2 >= 0) && (pointRelativeToEdges3 >= 0))
        || ((pointRelativeToEdges1 <= 0) && (pointRelativeToEdges2 <= 0) && (pointRelativeToEdges3 <= 0));
}

bool isConcave(const Point& a, const Point& b, const Point& c)
{
    double abx = b.x - a.x, aby = b.y - a.y, abz = b.z - a.z;
    double bcx = c.x - b.x, bcy = c.y - b.y, bcz = c.z - b.z;

    double crossProductX = aby * bcz - abz * bcy;
    double crossProductY = abz * bcx - abx * bcz;
    double crossProductZ = abx * bcy - aby * bcx;

    double aadsa = (crossProductX * c.x + crossProductY * c.y + crossProductZ * c.z);
    return aadsa > 0.0;
}

std::string earClippingTriangulation(const std::vector<Point>& polygon)
{
    std::string string = "";
    size_t n = polygon.size();

    if (n < 3)
    {
        std::cerr << "Not enough vertices for triangulation." << std::endl;
        return "";
    }

    std::vector<Point> polygonCopy = polygon;
    std::vector<Point> earVertex;

    int countConvex = 0;
    int countConcave = 0;

    for (size_t i = 0; i < polygonCopy.size(); i++)
    {
        size_t i0 = i % polygonCopy.size();
        size_t i1 = (i + 1) % polygonCopy.size();
        size_t i2 = (i + 2) % polygonCopy.size();

        const Point& v0 = polygonCopy[i0];
        const Point& v1 = polygonCopy[i1];
        const Point& v2 = polygonCopy[i2];

        if (isConcave(v0, v1, v2))
        {
            countConcave++;
        }
        else
        {
            countConvex++;
        }
    }

    int isConvexPolygon = -1;
    isConvexPolygon = countConvex > countConcave ? 1 : 0;
    int size = 0;

    while (polygonCopy.size() > 2)
    {
        int size1 = size;
        for (size_t i = 0; i < polygonCopy.size(); i++)
        {
            size_t i0 = i % polygonCopy.size();
            size_t i1 = (i + 1) % polygonCopy.size();
            size_t i2 = (i + 2) % polygonCopy.size();

            const Point& v0 = polygonCopy[i0];
            const Point& v1 = polygonCopy[i1];
            const Point& v2 = polygonCopy[i2];

            if ((isConcave(v0, v1, v2) && !isConvexPolygon) || (!isConcave(v0, v1, v2) && isConvexPolygon))
            {
                string += std::to_string(earVertex.size() + 1) + ",        " + std::to_string(v0.number) + ",        " + std::to_string(v1.number) + ",        " +
                    std::to_string(v2.number) + "\n";

                earVertex.push_back(polygonCopy[i1]);
                polygonCopy.erase(polygonCopy.begin() + i1);
                size++;
            }

            if (polygonCopy.size() <= 2)
            {
                break;
            }

        }

        if (size == size1)
        {
            break;
        }
    }
    return string;
}

int main(int argc, char* argv[])
{
    std::string inputstring = argv[2];
    std::string outputstring = argv[4];
    std::endl(std::cout);

    setlocale(LC_ALL, "rus");
    std::vector<Point> polygon;
    std::ifstream inputFile(inputstring);

    if (!inputFile.is_open())
    {
        std::cerr << "Unable to open the file." << std::endl;
        return 1;
    }

    std::string line;
    int countstring = 0;
    while (std::getline(inputFile, line))
    {
        if (countstring > 0)
        {
            std::replace(line.begin(), line.end(), ',', ' ');

            std::istringstream ss(line);
            Point p;
            ss >> p.number >> p.x >> p.y >> p.z;
            polygon.push_back(p);
        }
        countstring++;
    }

    inputFile.close();

    std::string string = earClippingTriangulation(polygon);

    std::fstream outputFile(outputstring); 


    if (!outputFile.is_open())
    {
        std::cerr << "Unable to open the file." << std::endl;
        return 1;
    }

    outputFile << "*   N,            X             Y             Z\n";
    outputFile << "*Nodes\n";

    std::ifstream inputFile2(inputstring);
    countstring = 0;

    while (std::getline(inputFile2, line))
    {
        if (countstring > 0)
        {
            outputFile << line << "\n";
        }
        countstring++;
    }

    outputFile << "*Elements\n";
    outputFile << string;

    outputFile.close();
    inputFile2.close();

    return 0;
}
