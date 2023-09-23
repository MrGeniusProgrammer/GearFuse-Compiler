class Object
{
private:
  const char *name;
  char c;
  int length;

public:
  Object(const char *name, int length) : name(name), length(length){};
  Object(char c, int length) : c(c), length(length){};
  int print();
};

int Object::print()
{
  return 1;
}

int main()
{
  Object object1("hello1", 12);
  Object object2('5', 12);
  Object *object3 = &object2;
  return 1;
}