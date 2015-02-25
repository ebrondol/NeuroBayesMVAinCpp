#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class sort_map
{
  public:
        string key;
        double val;
};

bool Sort_by(const sort_map& a ,const sort_map& b)
{
        return a.val > b.val;
}

void sorting(map<string, double> d)
{
        map<string,double>::iterator it;
        vector< sort_map > v;
        vector< sort_map >::iterator itv;
        sort_map sm;

	ofstream fom_summary ("results/fom_summary.txt");
	fom_summary << "Name\t\t|FoM rebinned\t\t" <<endl;
	ofstream fom_sorted ("results/fom_sorted.txt");
	fom_sorted << "Name\t\t|FoM rebinned\t\t" <<endl;

        for (it = d.begin(); it != d.end(); ++it)
        {
                sm.key = (*it).first; sm.val = (*it).second;
                v.push_back(sm);
        }

        for (itv = v.begin(); itv != v.end(); ++itv)
        {
                fom_summary << (*itv).key << " | " << (*itv).val << endl;
        }

        sort(v.begin(),v.end(),Sort_by);

        cout << "sorted" << endl;

        for (itv = v.begin(); itv != v.end(); ++itv)
        {
            fom_sorted << (*itv).key << " | " << (*itv).val << endl;
        }

        fom_summary.close();
        fom_sorted.close();

	cout << "-----------------------" << endl;
	cout << "FoM max: " << (*(v.begin())).val << " in " << (*(v.begin())).key<< endl;
        return;

}
