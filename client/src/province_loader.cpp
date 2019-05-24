#include "province_loader.h"


std::vector<std::unique_ptr<Province>> ProvinceLoader::Load(const unsigned char * pixels, int width, int height)
{
    std::vector<std::unique_ptr<Province>> provinces;
    
    loadFromFile(provinces);
    loadFromMap(provinces, pixels, width, height);
    
    return provinces;
}

void ProvinceLoader::loadFromFile(std::vector<std::unique_ptr<Province>> & provinces)
{
    bool log = false;
    namespace fs = std::experimental::filesystem;
    std::string path = "../shared/provinces/";
    for(auto & p : fs::directory_iterator(path))
    {
        std::string s = p.path().string();
        std::size_t txt = s.find(".txt");
        if (txt != std::string::npos)
        {
            std::size_t sl = s.find_last_of("/") + 1;
            std::string fileName = s.substr(sl, txt - sl);
            
            std::size_t floor = fileName.find("_");

            std::string provName = fileName.substr(0, floor);
            std::string provId = fileName.substr(floor + 1);

            if (provId.find("/") != std::string::npos)
                continue;

            if (log)
                std::cout << "provId: " << provId << std::endl;


            std::fstream file(path + provName + "_" + provId + ".txt", std::fstream::in);

            if (file.is_open())
            {
                DataLoader loader;
                DataObj * data = loader.LoadDataObj(file, "province");

                provinces.emplace_back(std::make_unique<Province>(std::stoi(provId), data));
                delete data;
                file.close();
            }
        }
    }
}

void ProvinceLoader::loadFromMap(std::vector<std::unique_ptr<Province>> & provinces, const unsigned char * pixels, int width, int height)
{
    // load horizontaly
    Color lastColor{255,255,255,255};
    for (int i = 0; i < width * height * 4 - 3; i += 4)
    {
        Color currentColor{ pixels[i + 0], pixels[i + 1], pixels[i + 2], pixels[i + 3] };

        if (currentColor != lastColor)
        {
            loadNeighbours(provinces, lastColor, currentColor, i);
            lastColor = currentColor;
        }
    }

    // load verticaly
    lastColor = Color{255,255,255,255};
    for (int x = 0; x < width * 4 - 3; x += 4)
    {
        for (int y = 0; y < height * 4 - 3; y += 4)
        {
            int index = y * width + x;
            Color currentColor{ pixels[index + 0], pixels[index + 1], pixels[index + 2], pixels[index + 3] };

            if (currentColor != lastColor)
            {
                loadNeighbours(provinces, lastColor, currentColor, index);
                lastColor = currentColor;
            }
        }
    }

#if 0
    for (auto & prov : provinces)
        {
            //if (prov.GetName() != "Przemysl") continue;
            std::cout << prov.GetName() << ", id: " << prov.GetId() << "\n";
            std::cout << "Neighbours\n";
            for (auto & n : prov.GetNeighbours())
                std::cout << "- " << "" << getProvinceName(provinces, n) << "\n";
            std::cout << "--------------------\n\n";
        }
    std::cout << "Num of provinces = " << provinces.size() << "\n\n";        
#endif
}

void ProvinceLoader::loadNeighbours(std::vector<std::unique_ptr<Province>> & provinces, Color lastColor, Color currentColor, int index)
{
    const Color whiteColor{255,255,255,255};
        
    if (lastColor == whiteColor)
        return;

    for (auto & prov : provinces)
    {
        if (prov->GetColor() == currentColor)
        {
            prov->AddNeighbour(getProvinceId(provinces, lastColor));
            for (auto & provv : provinces)
                if (provv->GetColor() == lastColor)
                {
                    provv->AddNeighbour(prov->GetId());
                    break;
                }
                
            break;
        }
    }
}

int ProvinceLoader::getProvinceId(const std::vector<std::unique_ptr<Province>> & provinces, Color color)
{
    for (auto & p : provinces)
        if (p->GetColor() == color)
            return p->GetId();
    return -1;
}
