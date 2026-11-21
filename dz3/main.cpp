  }

  AbstractFabricPlanar** makeFabricsPlanar()
  {
    AbstractFabricPlanar** fabricsPlanar = new AbstractFabricPlanar*[planarSize];
    size_t i = 0;
    try
    {
      fabricsPlanar[i] = new FabricPoint();
      i++;
      fabricsPlanar[i] = new FabricVector();
      i++;
      fabricsPlanar[i] = new FabricColoredPoint();
      i++;
      fabricsPlanar[i] = new FabricCircle();
      i++;
      fabricsPlanar[i] = new FabricColoredCircle();
      i++;
    }
    catch (const std::exception& e)
    {
      clearFabricsPlanar(fabricsPlanar, i);
      throw;
    }
    return fabricsPlanar;
  }
}

std::ostream& printAbsSqr(std::ostream& out, arch::Planar* pl)
{
  return pl->info(out) << ' ' << pl->abs_sqr();
}

void clear(arch::Planar* pls[], size_t size)
{
  for (size_t i = 0; i < size; i++)
  {
    delete pls[i];
  }
  delete[] pls;
}

int main()
{
  size_t cap = 0;
  std::cin >> cap;
  if (!std::cin)
  {
    std::cerr << "Incorrect size\n";
    return 1;
  }

  fabric::AbstractFabricPlanar** fabrics = nullptr;
  arch::Planar** pls = nullptr;
  size_t size = 0;
  try
  {
    fabrics = fabric::makeFabricsPlanar();
    pls = new arch::Planar*[cap];

    while (size < cap)
    {
      int num = 0;
      if (!(std::cin >> num))
      {
        throw std::logic_error("Incorrect planar");
      }
      bool found = false;
      for (size_t j = 0; j < planarSize; j++)
      {
        if (num == planarTypes[j])
        {
          pls[size] = fabrics[j]->create(std::cin);
          size++;
          found = true;
          break;
        }
      }
      if (!found)
      {
        throw std::logic_error("Incorrect planar");
      }
    }

    for (size_t i = 0; i < size; i++)
    {
      printAbsSqr(std::cout, pls[i]) << '\n';
    }
  }
  catch (const std::exception& e)
  {
    if (fabrics != nullptr)
    {
      fabric::clearFabricsPlanar(fabrics, planarSize);
    }
    clear(pls, size);
    std::cerr << e.what() << '\n';
    return 1;
  }

  fabric::clearFabricsPlanar(fabrics, planarSize);
  clear(pls, size);
  return 0;
