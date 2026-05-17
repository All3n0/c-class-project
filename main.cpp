#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <limits>
#include <algorithm>
using namespace std;
//input helpers section
// Read an integer within [lo, hi], re-prompting until valid
int readInt(const string& prompt, int lo, int hi) {
    int val;
    while (true) {
        cout << prompt;
        if (cin >> val && val >= lo && val <= hi) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  [!] Enter a number between " << lo << " and " << hi << "\n";
    }
}

// Read a float > 0, re-prompting until valid
float readPositiveFloat(const string& prompt) {
    float val;
    while (true) {
        cout << prompt;
        if (cin >> val && val > 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  [!] Enter a value greater than 0\n";
    }
}

// Read a non-empty string
string readString(const string& prompt) {
    string val;
    while (true) {
        cout << prompt;
        getline(cin, val);
        if (!val.empty()) return val;
        cout << "  [!] This field cannot be empty\n";
    }
}

// Show a numbered option menu and return the chosen 1-based index
int pickOption(const string& title, const vector<string>& options) {
    cout << "\n  " << title << "\n";
    for (int i = 0; i < (int)options.size(); i++)
        cout << "  " << (i + 1) << ". " << options[i] << "\n";
    return readInt("  Choice: ", 1, (int)options.size());
}

// Item categories : single source of truth
const vector<string> CATEGORIES = {
    "GPU", "CPU", "RAM", "Motherboard",
    "phone", "laptop", "tablet", "accessory",
    "Li-battery", "UPS"
};

string pickCategory() {
    int c = pickOption("Select item category:", CATEGORIES);
    return CATEGORIES[c - 1];
}

// Zone IDs per category — maps category to correct zone
string zoneForCategory(const string& cat) {
    if (cat == "GPU")         return "F4-A";
    if (cat == "CPU")         return "F4-B";
    if (cat == "RAM")         return "F4-C";
    if (cat == "Motherboard") return "F4-D";
    if (cat == "phone")       return "F3-A";
    if (cat == "laptop")      return "F3-B";
    if (cat == "tablet")      return "F3-C";
    if (cat == "accessory")   return "F3-D";
    if (cat == "Li-battery")  return "F2-A";
    if (cat == "UPS")         return "F2-C";
    return "F1-A";
}

char pickCondition() {
    int c = pickOption("Select condition:",
        {"G : Good (crate exterior intact)",
         "D : Damaged (crate visibly damaged or non-compliant)",
         "S : Suspect (minor concerns, needs closer check)"});
    if (c == 1) return 'G';
    if (c == 2) return 'D';
    return 'S';
}

char pickOrderStatus() {
    int c = pickOption("Select new status:",
        {"P  Pending",
         "R  Received",
         "D  Dispatched",
         "C  Cancelled"});
    if (c == 1) return 'P';
    if (c == 2) return 'R';
    if (c == 3) return 'D';
    return 'C';
}

// All known zone IDs
const vector<string> ALL_ZONES = {
    "F1-A (Receiving bay)","F1-B (Dispatch bay)","F1-C (Returns bay)","F2-A (Li-battery)","F2-B (Phone battery stock)","F2-C (UPS units)",
    "F3-A (Phones)","F3-B (Laptops)","F3-C (Tablets)","F3-D (Accessories)","F4-A (GPUs : vault)","F4-B (CPUs : vault)","F4-C (RAM : vault)","F4-D (Motherboards : vault)"
};
const vector<string> ZONE_IDS = {
    "F1-A","F1-B","F1-C",
    "F2-A","F2-B","F2-C",
    "F3-A","F3-B","F3-C","F3-D",
    "F4-A","F4-B","F4-C","F4-D"
};

string pickZone() {
    int c = pickOption("Select zone:", ALL_ZONES);
    return ZONE_IDS[c - 1];
}

int pickFloor() {
    int c = pickOption("Select floor:",
        {"Floor 1 : Receiving / Dispatch / Returns",
         "Floor 2 : Battery & Hazardous Stock",
         "Floor 3 : Consumer Electronics",
         "Floor 4 : High-Value Components (vault)"});
    return c;
}

string pickStaff(const vector<pair<string,string>>& staff) {
    cout << "\n  Select staff member:\n";
    for (int i = 0; i < (int)staff.size(); i++)
        cout << "  " << (i+1) << ". " << staff[i].first
             << " || " << staff[i].second << "\n";
    int c = readInt("  Choice: ", 1, (int)staff.size());
    return staff[c-1].first;
}
//  DATE
class Date {
private:
    int day, month, year;
public:
    Date(int d, int m, int y) : day(d), month(m), year(y) {}
    string toString() const {
        return to_string(day) + "/" + to_string(month) + "/" + to_string(year);
    }
    bool operator==(const Date& o) const {
        return day==o.day && month==o.month && year==o.year;
    }
    int getDay()   const { return day;   }
    int getMonth() const { return month; }
    int getYear()  const { return year;  }
};

//  STORAGE UNIT — base class (Inheritance)
class StorageUnit {
protected:
    string unitId;
    string unitName;
public:
    StorageUnit(string id, string name) : unitId(id), unitName(name) {}
    virtual ~StorageUnit() {}
    string getId()   const { return unitId;   }
    string getName() const { return unitName; }
    virtual void printStatus() const = 0;
    virtual int  totalFree()   const = 0;
};
// Bay 
class Bay : public StorageUnit {
private:
    int capacity;
    int occupied;
public:
    Bay(string id, int cap)
        : StorageUnit(id, "Bay " + id), capacity(cap), occupied(0) {}
    bool isOpen()    const { return occupied < capacity; }
    int  freeSlots() const { return capacity - occupied; }
    int  totalFree() const override { return freeSlots(); }
    bool addItem() {
        if (occupied >= capacity) return false;
        occupied++; return true;
    }
    bool removeItem() {
        if (occupied <= 0) return false;
        occupied--; return true;
    }
    void printStatus() const override {
        cout << "      Bay " << setw(8) << left << unitId
             << "  " << occupied << "/" << capacity
             << (isOpen() ? "  [OPEN]" : "  [FULL]") << "\n";
    }
};
// Sensor 
class Sensor {
private:
    string sensorId, zone;
    float  tempC, humidityPct;
    bool   smokeDetected, alertActive;
    float  maxTempC, maxHumidity;
public:
    Sensor(string id, string z, float maxT, float maxH)
        : sensorId(id), zone(z), tempC(0), humidityPct(0),
          smokeDetected(false), alertActive(false),
          maxTempC(maxT), maxHumidity(maxH) {}

    string update(float t, float h, bool smoke) {
        tempC = t; humidityPct = h; smokeDetected = smoke;
        alertActive = false;
        if (smoke) {
            alertActive = true;
            return "[FIRE ALERT]     Zone " + zone + " : smoke detected!";
        }
        if (t > maxTempC) {
            alertActive = true;
            return "[TEMP ALERT]     Zone " + zone + " : " +
                   to_string((int)t) + "C exceeds max " +
                   to_string((int)maxTempC) + "C";
        }
        if (h > maxHumidity) {
            alertActive = true;
            return "[HUMIDITY ALERT] Zone " + zone + " : " +
                   to_string((int)h) + "% exceeds max " +
                   to_string((int)maxHumidity) + "%";
        }
        return "";
    }

    string statusLine() const {
        return "    " + sensorId +
               "  Temp: "     + to_string((int)tempC) + "C" +
               "  Humidity: " + to_string((int)humidityPct) + "%" +
               "  Smoke: "    + (smokeDetected ? "YES" : "no") +
               "  Status: "   + (alertActive ? "** ALERT **" : "OK");
    }
    bool isAlert() const { return alertActive; }
};
// Zone 
class Zone : public StorageUnit {
private:
    string      category;
    float       tempMin, tempMax, maxHumidity;
    bool        antiStatic, restricted;
    vector<Bay> bays;
public:
    Sensor sensor;
    Zone(string id, string cat, float tMin, float tMax,float maxH, bool anti, bool restr, int numBays, int bayCapacity)
        : StorageUnit(id, "Zone " + id),
          category(cat), tempMin(tMin), tempMax(tMax),
          maxHumidity(maxH), antiStatic(anti), restricted(restr),
          sensor("S-" + id, id, tMax, maxH) {
        for (int i = 1; i <= numBays; i++) {
            string bid = id + "-" + (i < 10 ? "0" : "") + to_string(i);
            bays.push_back(Bay(bid, bayCapacity));
        }
    }
    string getCategory()   const { return category;   }
    bool   isRestricted()  const { return restricted; }
    int totalFree() const override {
        int t = 0;
        for (const Bay& b : bays) t += b.freeSlots();
        return t;
    }
    bool assignItem(string& outBayId) {
        for (Bay& b : bays) {
            if (b.isOpen()) { b.addItem(); outBayId = b.getId(); return true; }
        }
        return false;
    }
    bool isSuitableFor(const string& cat) const { return cat == category; }
    void printStatus() const override {
        cout << "    Zone " << setw(6) << left << unitId
             << "  [" << setw(12) << left << category << "]"
             << "  Free: " << setw(4) << totalFree()
             << (restricted ? "  [RESTRICTED]" : "") << "\n";
        for (const Bay& b : bays) b.printStatus();
    }
};
// Floor
class Floor : public StorageUnit {
private:
    int          floorNumber;
    vector<Zone> zones;
public:
    Floor(int num, string name)
        : StorageUnit("F" + to_string(num), name), floorNumber(num) {}
    int  getFloorNumber() const { return floorNumber; }
    void addZone(Zone z) { zones.push_back(z); }
    string assignItem(const string& category) {
        for (Zone& z : zones) {
            if (z.isSuitableFor(category)) {
                string bayId;
                if (z.assignItem(bayId)) return bayId;
                return "ZONE_FULL";
            }
        }
        return "NO_ZONE";
    }
    Zone* findZone(const string& id) {
        for (Zone& z : zones) if (z.getId() == id) return &z;
        return nullptr;
    }
    int totalFree() const override {
        int t = 0;
        for (const Zone& z : zones) t += z.totalFree();
        return t;
    }
    void printStatus() const override {
        cout << "\n  Floor " << floorNumber << ": " << unitName << "\n";
        cout << "  " << string(56, '-') << "\n";
        for (const Zone& z : zones) z.printStatus();
    }
    vector<Zone>& getZones() { return zones; }
};
//  STOCK ITEM — Encapsulation
class StockItem {
private:
    string serialNumber, crateId, model, category;
    char   condition;
    bool   hasBattery;
    float  price;
    int    warrantyMonths;
    string assignedLocation;
    Date   dateReceived;
public:
    StockItem(string sn, string cid, string mdl, string cat,
              float p, int wm, Date d)
        : serialNumber(sn), crateId(cid), model(mdl), category(cat),
          condition('G'), price(p), warrantyMonths(wm),
          assignedLocation("UNASSIGNED"), dateReceived(d) {
        hasBattery = (cat=="Li-battery"||cat=="phone"||cat=="laptop"||cat=="UPS");
    }
    string getSerial()    const { return serialNumber;    }
    string getCrateId()   const { return crateId;         }
    string getModel()     const { return model;           }
    string getCategory()  const { return category;        }
    char   getCondition() const { return condition;       }
    bool   getHasBattery()const { return hasBattery;      }
    float  getPrice()     const { return price;           }
    string getLocation()  const { return assignedLocation;}
    Date   getDate()      const { return dateReceived;    }
    void   setLocation(const string& l) { assignedLocation = l; }
    void   setCondition(char c)         { condition = c;        }

    void print() const {
        cout << "  " << setw(18) << left << serialNumber
             << "  " << setw(12) << left << category
             << "  Loc: " << setw(10) << left << assignedLocation
             << "  KES " << setw(8) << right << (int)price
             << "  Cond: " << condition
             <<(condition=='R'? "[RESERVED]":"")
             << (hasBattery ? "  [BATTERY]" : "") << "\n";
    }
};
//  CRATE
class Crate {
private:
    string crateId, supplierId, poId, category;
    char   status, condition;
    int    unitCount;
    string inspectedBy, assignedLocation;
    Date   dateArrived;
public:
    Crate(string id, string sup, string po, string cat,
          int units, char cond, string staff, Date d)
        : crateId(id), supplierId(sup), poId(po), category(cat),
          status(cond=='D'?'R':'A'), condition(cond),
          unitCount(units), inspectedBy(staff),
          assignedLocation("UNASSIGNED"), dateArrived(d) {}
    string getId()        const { return crateId;    }
    string getSupplier()  const { return supplierId; }
    string getCategory()  const { return category;   }
    char   getCondition() const { return condition;  }
    int    getUnitCount() const { return unitCount;  }
    char   getStatus()    const { return status;     }
    string getLocation()  const { return assignedLocation; }
    void   setLocation(const string& l) { assignedLocation = l; } 
    // Auto-generate serial numbers from crate ID
    vector<StockItem> generateItems(float unitPrice, int warrantyMonths) {
        vector<StockItem> items;
        for (int i = 1; i <= unitCount; i++) {
            string suf = to_string(i);
            while ((int)suf.length() < 3) suf = "0" + suf;
            StockItem item(crateId + "-" + suf, crateId,
                           crateId + "-model", category,
                           unitPrice, warrantyMonths, dateArrived);
            item.setCondition(condition);
            items.push_back(item);
        }
        if (condition != 'D') status = 'C';
        return items;
    }
};
//  ORDER 
class Order {
protected:
    string orderId, partyId;
    int    quantity;
    float  totalValue;
    char   status;
    Date   orderDate;
public:
    Order(string id, string party, int qty, float val, Date d)
        : orderId(id), partyId(party), quantity(qty),
          totalValue(val), status('P'), orderDate(d) {}
    virtual ~Order() {}
    string getId()     const { return orderId;    }
    string getParty()  const { return partyId;    }
    int    getQty()    const { return quantity;   }
    float  getValue()  const { return totalValue; }
    char   getStatus() const { return status;     }
    void   setStatus(char s) { status = s;        }
    virtual string getType() const = 0;
    virtual void   print()   const = 0;
    string statusLabel() const {
        if (status=='P') return "Pending";
        if (status=='R') return "Received";
        if (status=='D') return "Dispatched";
        return "Cancelled";
    }
};
class InboundOrder : public Order {
private:
    string supplierId;
public:
    InboundOrder(string id, string sup, int qty, float val, Date d)
        : Order(id, sup, qty, val, d), supplierId(sup) {}
    string getType() const override { return "INBOUND"; }
    void print() const override {
        cout << "  [IN]  " << setw(14) << left << orderId
             << "  Supplier: " << setw(14) << left << supplierId
             << "  Qty: " << setw(4) << quantity
             << "  KES " << setw(10) << right << (int)totalValue
             << "  " << statusLabel() << "\n";
    }
};
class OutboundOrder : public Order {
private:
    string customerId;
public:
    OutboundOrder(string id, string cust, int qty, float val, Date d)
        : Order(id, cust, qty, val, d), customerId(cust) {}
    string getType() const override { return "OUTBOUND"; }
    void print() const override {
        cout << "  [OUT] " << setw(14) << left << orderId
             << "  Customer: " << setw(14) << left << customerId
             << "  Qty: " << setw(4) << quantity
             << "  KES " << setw(10) << right << (int)totalValue
             << "  " << statusLabel() << "\n";
    }
};
//  REPORT — base + derived
class Report {
protected:
    string reportId, generatedBy, content;
    Date   date;
public:
    Report(string id, string by, Date d, string body)
        : reportId(id), generatedBy(by), content(body), date(d) {}
    virtual ~Report() {}
    string getId()      const { return reportId; }
    string getContent() const { return content;  }
    virtual string getType() const = 0;
    virtual void   print()   const = 0;
};
class GRNReport : public Report {
private:
    int accepted, rejected;
public:
    GRNReport(string id, string by, Date d,
              string sup, int recv, int rej)
        : Report(id, by, d,
                 "Supplier: " + sup + " | Accepted: " + to_string(recv) +
                 " | Rejected: " + to_string(rej)),
          accepted(recv), rejected(rej) {}
    string getType() const override { return "GRN"; }
    void print() const override {
        cout << "  [GRN]    " << reportId << "  " << content
             << "  (" << date.toString() << ")\n";
    }
};
class DefectReport : public Report {
public:
    DefectReport(string id, string by, Date d,
                 string sup, string model, string desc)
        : Report(id, by, d,
                 "Supplier: " + sup + " | Model: " + model +
                 " | " + desc) {}
    string getType() const override { return "Defect"; }
    void print() const override {
        cout << "  [DEFECT] " << reportId << "  " << content
             << "  (" << date.toString() << ")\n";
    }
};
class FireAlert : public Report {
public:
    FireAlert(string id, string by, Date d,
              string zone, int battCount)
        : Report(id, by, d,
                 "FIRE — Zone: " + zone +
                 " | Battery items: " + to_string(battCount)) {}
    string getType() const override { return "FireAlert"; }
    void print() const override {
        cout << "  [FIRE]   " << reportId << "  " << content
             << "  (" << date.toString() << ")\n";
    }
};
class EnvAlert : public Report {
public:
    EnvAlert(string id, string by, Date d,
             string zone, float t, float h, string breach)
        : Report(id, by, d,
                 "ENV — Zone: " + zone +
                 " | Temp: " + to_string((int)t) + "C" +
                 " | Humidity: " + to_string((int)h) + "%" +
                 " | Breach: " + breach) {}
    string getType() const override { return "EnvAlert"; }
    void print() const override {
        cout << "  [ENV]    " << reportId << "  " << content
             << "  (" << date.toString() << ")\n";
    }
};
//  SHIFT + ACCESS LOG
class Shift {
private:
    string staffId, staffName, role, zone;
    char   accessLevel;
    int    startHour, endHour;
    Date   shiftDate;
public:
    Shift(string id, string name, string r, string z,
          char access, int start, int end, Date d)
        : staffId(id), staffName(name), role(r), zone(z),
          accessLevel(access), startHour(start), endHour(end), shiftDate(d) {}
    string getId()    const { return staffId;     }
    string getName()  const { return staffName;   }
    string getRole()  const { return role;        }
    string getZone()  const { return zone;        }
    char   getAccess()const { return accessLevel; }
    int    getStart() const { return startHour;   }
    int    getEnd()   const { return endHour;     }
};
class AccessLog {
private:
    string logId, staffId, zone;
    bool   granted;
    int    hourAttempted;
public:
    AccessLog(string lid, string sid, string z, bool g, int hr)
        : logId(lid), staffId(sid), zone(z), granted(g), hourAttempted(hr) {}
    bool   isGranted()  const { return granted;   }
    string getStaffId() const { return staffId;   }
    void print() const {
        cout << "  " << setw(10) << left << logId
             << "  Staff: " << setw(6)  << left << staffId
             << "  Zone: "  << setw(6)  << left << zone
             << "  Hour: "  << setw(3)  << hourAttempted
             << ":00  " << (granted ? "[GRANTED]" : "[DENIED]") << "\n";
    }
};
//  DEPARTMENT — base class
class Department {
protected:
    string departmentId;
    Date   today;
public:
    Department(string id, Date d) : departmentId(id), today(d) {}
    virtual ~Department() {}
    string getId() const { return departmentId; }
    virtual string getName()      const = 0;
    virtual void   printSummary() const = 0;
};
//  InventoryControl
class InventoryControl : public Department {
private:
    vector<StockItem> stock;
    int               lowStockThreshold;
public:
    InventoryControl(string id, int thr, Date d)
        : Department(id, d), lowStockThreshold(thr) {}
    string getName() const override { return "Inventory Control"; }
    bool addItem(StockItem item) { stock.push_back(item); return true; }
    bool removeItem(const string& serial) {
        for (int i = 0; i < (int)stock.size(); i++) {
            if (stock[i].getSerial() == serial) {
                stock.erase(stock.begin() + i); return true;
            }
        }
        return false;
    }

    int getStockCount(const string& model) const {
        int c = 0;
        for (const StockItem& s : stock) if (s.getModel() == model) c++;
        return c;
    }

    vector<StockItem> getBatteryItems() const {
        vector<StockItem> r;
        for (const StockItem& s : stock) if (s.getHasBattery()) r.push_back(s);
        return r;
    }

    int totalItems() const { return (int)stock.size(); }
    const vector<StockItem>& getStock() const { return stock; }
    vector<StockItem>& getStockMutable() { return stock; }

    void printSummary() const override {
        cout << "\n  Inventory — " << stock.size() << " item(s)\n";
        cout << "  " << string(72, '-') << "\n";
        if (stock.empty()) { cout << "  (no stock)\n"; return; }
        for (const StockItem& s : stock) s.print();
    }
};
//  Receiving 
class Receiving : public Department {
private:
    vector<Crate>     crateLog;
    vector<GRNReport> grnLog;
    int               counter;
public:
    Receiving(string id, Date d) : Department(id, d), counter(1000) {}
    string getName() const override { return "Receiving"; }

    GRNReport processCrate(Crate& crate) {
        int acc = (crate.getCondition() == 'D') ? 0 : crate.getUnitCount();
        int rej = (crate.getCondition() == 'D') ? crate.getUnitCount() : 0;
        GRNReport grn("GRN-" + to_string(counter++), departmentId, today,
                      crate.getSupplier(), acc, rej);
        grnLog.push_back(grn);
        crateLog.push_back(crate);
        return grn;
    }

    DefectReport flagDefect(const string& sup,
                            const string& model,
                            const string& desc) {
        return DefectReport("DEF-" + to_string(counter++),
                            departmentId, today, sup, model, desc);
    }

    void printSummary() const override {
        cout << "\n  Receiving — " << crateLog.size() << " crate(s)\n";
        cout << "  " << string(72, '-') << "\n";
        for (const GRNReport& g : grnLog) g.print();
    }
};
//  Dispatch 
class Dispatch : public Department {
private:
    vector<OutboundOrder*> orders;
    int counter;
public:
    Dispatch(string id, Date d) : Department(id, d), counter(1) {}
    string getName() const override { return "Dispatch"; }

    OutboundOrder* createOrder(const string& oid,
                               const string& cid,
                               int qty, float val) {
        OutboundOrder* o = new OutboundOrder(oid, cid, qty, val, today);
        orders.push_back(o);
        return o;
    }
    bool fulfillOrder(const string& oid) {
        for (OutboundOrder* o : orders) {
            if (o->getId() == oid) { o->setStatus('D'); return true; }
        }
        return false;
    }
    void printSummary() const override {
        cout << "\n  Dispatch  " << orders.size() << " outbound order(s)\n";
        cout << "  " << string(72, '-') << "\n";
        for (const OutboundOrder* o : orders) o->print();
    }

    ~Dispatch() { for (OutboundOrder* o : orders) delete o; }
};
// Schedule + Security 
class Schedule {
private:
    map<string, vector<Shift>> shiftMap;
public:
    void addShift(Shift s) {
        for (int h = s.getStart(); h < s.getEnd(); h++) {
            shiftMap[s.getZone() + ":" + to_string(h)].push_back(s);
        }
    }
    vector<Shift> getOnDuty(const string& zone, int hour) const {
        auto it = shiftMap.find(zone + ":" + to_string(hour));
        if (it != shiftMap.end()) return it->second;
        return {};
    }
    void printOnDuty(int hour) const {
        cout << "\n  Guards on duty at " << hour << ":00\n";
        cout << "  " << string(56, '-') << "\n";
        bool found = false;
        map<string,bool> seen;
        for (auto& entry : shiftMap) {
            int colon = (int)entry.first.rfind(':');
            if (stoi(entry.first.substr(colon+1)) != hour) continue;
            string zone = entry.first.substr(0, colon);
            for (const Shift& s : entry.second) {
                if (seen[s.getId()+zone]) continue;
                seen[s.getId()+zone] = true;
                cout << "  " << setw(16) << left << s.getName()
                     << "  Role: " << setw(12) << left << s.getRole()
                     << "  Zone: " << setw(6)  << left << zone
                     << "  Access: " << s.getAccess() << "\n";
                found = true;
            }
        }
        if (!found) cout << "  No shifts registered for this hour.\n";
    }
};
class Security : public Department {
private:
    Schedule&          schedule;
    vector<AccessLog>  accessLog;
    int                logCounter;
public:
    Security(string id, Schedule& sched, Date d)
        : Department(id, d), schedule(sched), logCounter(1) {}
    string getName() const override { return "Security"; }

    AccessLog attemptEntry(const string& staffId,
                           const string& zone, int hour) {
        bool granted = false;
        for (const Shift& s : schedule.getOnDuty(zone, hour))
            if (s.getId() == staffId) { granted = true; break; }
        if (!granted)
            for (const Shift& s : schedule.getOnDuty("ALL", hour))
                if (s.getId() == staffId && s.getAccess()=='A')
                    { granted = true; break; }
        AccessLog entry("LOG-" + to_string(logCounter++),
                        staffId, zone, granted, hour);
        accessLog.push_back(entry);
        return entry;
    }
    int totalDenied() const {
        int c = 0;
        for (const AccessLog& l : accessLog) if (!l.isGranted()) c++;
        return c;
    }
    void printSummary() const override {
        cout << "\n  Security — " << accessLog.size() << " attempt(s)\n";
        cout << "  " << string(72, '-') << "\n";
        for (const AccessLog& l : accessLog) l.print();
        cout << "  Total denied today: " << totalDenied() << "\n";
    }
};


// ── EnvironmentalMonitoring ───────────────────────────────────────
class EnvironmentalMonitoring : public Department {
private:
    vector<Report*> alertLog;
    int             counter;
public:
    EnvironmentalMonitoring(string id, Date d)
        : Department(id, d), counter(1) {}
    string getName() const override { return "Environmental Monitoring"; }
    void pushReading(Zone& zone, float tempC, float humidity, bool smoke) {
        string msg = zone.sensor.update(tempC, humidity, smoke);
        if (msg != "") {
            cout << "\n  !! " << msg << "\n";
            string id = "ALT-" + to_string(counter++);
            Report* r = smoke
                ? (Report*) new FireAlert(id, departmentId, today, zone.getId(), 0)
                : (Report*) new EnvAlert(id, departmentId, today,
                      zone.getId(), tempC, humidity,
                      tempC > 0 ? "Temperature" : "Humidity");
            alertLog.push_back(r);
        }
    }
    void printAllSensors(vector<Floor>& floors) const {
        cout << "\n  Sensor Status : All Zones\n";
        cout << "  " << string(72, '-') << "\n";
        for (Floor& f : floors) {
            cout << "  Floor " << f.getFloorNumber()
                 << ": " << f.getName() << "\n";
            for (Zone& z : f.getZones())
                cout << z.sensor.statusLine() << "\n";
        }
    }
    int totalAlerts() const { return (int)alertLog.size(); }
    void printSummary() const override {
        cout << "\n  Environmental Monitoring : "
             << alertLog.size() << " alert(s)\n";
        cout << "  " << string(72, ':') << "\n";
        for (const Report* r : alertLog) r->print();
    }
    ~EnvironmentalMonitoring() { for (Report* r : alertLog) delete r; }
};
//  ReturnsAndQA
class ReturnsAndQA : public Department {
private:
    vector<DefectReport> defectReports;
    int                  counter;
public:
    ReturnsAndQA(string id, Date d) : Department(id, d), counter(1) {}
    string getName() const override { return "Returns & QA"; }
    char assessItem(const StockItem& item) const {
        if (item.getCondition()=='G') return 'R';
        if (item.getCondition()=='D') return 'S';
        return 'I';
    }
    DefectReport raiseDefect(const string& sup,
                             const string& model,
                             const string& desc) {
        DefectReport r("QA-" + to_string(counter++),
                       departmentId, today, sup, model, desc);
        defectReports.push_back(r);
        return r;
    }
    void printSummary() const override {
        cout << "\n  Returns & QA : " << defectReports.size() << " defect(s)\n";
        cout << "  " << string(72, '-') << "\n";
        for (const DefectReport& r : defectReports) r.print();
    }
};
//  DAILY DASHBOARD
class DailyDashboard {
private:
    Date           date;
    vector<Order*> allOrders;
    float          totalCost, totalRevenue;
public:
    DailyDashboard(Date d)
        : date(d), totalCost(0), totalRevenue(0) {}

    void addOrder(Order* o) {
        allOrders.push_back(o);
        if (o->getType()=="INBOUND")  totalCost    += o->getValue();
        if (o->getType()=="OUTBOUND") totalRevenue += o->getValue();
    }
    int countByStatus(char s) const {
        int c = 0;
        for (const Order* o : allOrders) if (o->getStatus()==s) c++;
        return c;
    }
    void print() const {
        cout << "\n  +--------------------------------------------+\n";
        cout << "    WAREHOUSE DASHBOARD  " << date.toString()       << "\n";
        cout << "  +--------------------------------------------+\n";
        cout << "  Total orders   : " << allOrders.size()            << "\n";
        cout << "  Pending        : " << countByStatus('P')          << "\n";
        cout << "  Received       : " << countByStatus('R')          << "\n";
        cout << "  Dispatched     : " << countByStatus('D')          << "\n";
        cout << "  Cancelled      : " << countByStatus('C')          << "\n";
        cout << "  --------------------------------------------\n";
        cout << "  Stock cost     : KES " << fixed
             << setprecision(0) << totalCost                          << "\n";
        cout << "  Revenue        : KES " << totalRevenue             << "\n";
        cout << "  Net            : KES "
             << (totalRevenue - totalCost)                            << "\n";
        cout << "  +--------------------------------------------+\n";
        if (!allOrders.empty()) {
            cout << "\n  Orders:\n";
            for (const Order* o : allOrders) o->print();
        }
    }
    vector<Order*>& getOrders() { return allOrders; }
};
//  WAREHOUSE
class Warehouse {
public:
    string                  warehouseName;
    vector<Floor>           floors;
    DailyDashboard          dashboard;
    InventoryControl        inventory;
    Receiving               receiving;
    Dispatch                dispatch;
    ReturnsAndQA            returnsQA;
    EnvironmentalMonitoring envMonitor;
    Schedule                schedule;
    Security                security;

  vector<pair<string,string>> staffList;
    Warehouse(string name, Date today)
        : warehouseName(name),
          dashboard(today),
          inventory("INV-01", 5,  today),
          receiving("RCV-01",     today),
          dispatch ("DSP-01",     today),
          returnsQA("QA-01",      today),
          envMonitor("ENV-01",    today),
          security("SEC-01", schedule, today) {
        buildFloors();
        buildSchedule(today);
        buildStaffList();
    }
    void buildFloors() {
    // FLOOR 1: Receiving/Dispatch/Returns (high throughput, moderate storage)
    Floor f1(1, "Receiving / Dispatch / Returns");
    f1.addZone(Zone("F1-A", "RECEIVING", 10, 35, 80, false, false, 4, 20));  // 80 slots
    f1.addZone(Zone("F1-B", "DISPATCH", 10, 35, 80, false, false, 4, 20));    // 80 slots
    f1.addZone(Zone("F1-C", "RETURNS", 10, 35, 80, false, false, 3, 15));     // 45 slots
    floors.push_back(f1);  // Total Floor 1: ~205 slots

    // FLOOR 2: Battery & Hazardous (strict limits, smaller capacity)
    Floor f2(2, "Battery & Hazardous Stock");
    f2.addZone(Zone("F2-A", "Li-battery", 10, 20, 40, false, true, 4, 25));   // 100 slots
    f2.addZone(Zone("F2-B", "phone", 10, 20, 40, false, true, 3, 20));        // 60 slots
    f2.addZone(Zone("F2-C", "UPS", 10, 20, 40, false, true, 3, 20));          // 60 slots
    floors.push_back(f2);  // Total Floor 2: ~220 slots

    // FLOOR 3: Consumer Electronics (main storage, reasonable capacity)
    Floor f3(3, "Consumer Electronics");
    f3.addZone(Zone("F3-A", "phone", 15, 25, 45, false, false, 5, 30));       // 150 slots
    f3.addZone(Zone("F3-B", "laptop", 15, 25, 45, false, false, 5, 30));      // 150 slots
    f3.addZone(Zone("F3-C", "tablet", 15, 25, 45, false, false, 4, 30));      // 120 slots
    f3.addZone(Zone("F3-D", "accessory", 15, 25, 45, false, false, 4, 30));   // 120 slots
    floors.push_back(f3);  // Total Floor 3: ~540 slots

    // FLOOR 4: High-Value Components (vault - premium, limited space)
    Floor f4(4, "High-Value Components (vault)");
    f4.addZone(Zone("F4-A", "GPU", 18, 22, 35, true, true, 4, 20));           // 80 slots
    f4.addZone(Zone("F4-B", "CPU", 18, 22, 35, true, true, 4, 20));           // 80 slots
    f4.addZone(Zone("F4-C", "RAM", 18, 22, 35, true, true, 3, 20));           // 60 slots
    f4.addZone(Zone("F4-D", "Motherboard", 18, 22, 35, true, true, 3, 20));   // 60 slots
    floors.push_back(f4);  // Total Floor 4: ~280 slots
}
    void buildSchedule(Date today) {
        schedule.addShift(Shift("G001","James Otieno", "Guard","F1-A",'B',8,16,today));
        schedule.addShift(Shift("G002","Mary Wanjiku", "Guard","F1-B",'B',8,16,today));
        schedule.addShift(Shift("G003","Peter Kamau",  "Guard","F2-A",'B',8,16,today));
        schedule.addShift(Shift("G004","Amina Hassan", "Guard","F3-A",'B',8,16,today));
        schedule.addShift(Shift("G005","Brian Mwangi", "Guard","F4-A",'V',8,16,today));
        schedule.addShift(Shift("G006","Susan Chebet", "Guard","F1-A",'B',16,24,today));
        schedule.addShift(Shift("G007","David Ochieng","Guard","F2-A",'B',16,24,today));
        schedule.addShift(Shift("G008","Faith Njeri",  "Guard","F4-A",'V',16,24,today));
        schedule.addShift(Shift("G009", "John Mwangi", "Guard", "F4-B", 'V', 8, 16, today));
        schedule.addShift(Shift("G010", "Grace Atieno", "Guard", "F3-A", 'B', 0, 8, today));
        schedule.addShift(Shift("G011", "Grace Atieno", "Guard", "F3-B", 'B', 0, 8, today));
        schedule.addShift(Shift("S002", "James Mwangi", "Supervisor", "ALL", 'A', 18, 6, today));
        schedule.addShift(Shift("S001","Dr. Kamau",    "Supervisor", "ALL", 'A',6,18,today));
    }
    void buildStaffList() {
        staffList = {
            {"G001","James Otieno  : Guard F1-A  (day)"},
            {"G002","Mary Wanjiku  : Guard F1-B  (day)"},
            {"G003","Peter Kamau   : Guard F2-A  (day)"},
            {"G004","Amina Hassan  : Guard F3-A  (day)"},
            {"G005","Brian Mwangi  : Guard F4-A  (day, vault)"},
            {"G006","Susan Chebet  : Guard F1-A  (night)"},
            {"G007","David Ochieng : Guard F2-A  (night)"},
            {"G008","Faith Njeri   : Guard F4-A  (night, vault)"},
            {"G009", "John Mwangi  : Guard F4-B  (day)"},
            {"G010", "Grace Atieno : Guard F3-A  (night)"},
            {"G011", "Grace Atieno : Guard F3-B  (night)"},
            {"S002","James Mwangi  : Supervisor ALL (night)"},
            {"S001","Dr. Kamau     : Supervisor ALL (day)"}
        };
    }
    string assignLocation(StockItem& item) {
        for (Floor& f : floors) {
            string r = f.assignItem(item.getCategory());
            if (r != "NO_ZONE" && r != "ZONE_FULL") {
                item.setLocation(r);
                inventory.addItem(item);
                return r;
            }
        }
        return "UNASSIGNED";
    }
    Zone* findZone(const string& id) {
        for (Floor& f : floors) {
            Zone* z = f.findZone(id);
            if (z) return z;
        }
        return nullptr;
    }
    void printManagerView() {
        cout << "\n  ==========================================\n";
        cout << "   MANAGER VIEW — " << warehouseName << "\n";
        cout << "  ==========================================\n";
        for (const Floor& f : floors) f.printStatus();
        cout << "\n  Total items in warehouse: "
             << inventory.totalItems() << "\n";
        for (int i = 1; i <= 4; i++)
            cout << "  Free slots Floor " << i << ": "
                 << floors[i-1].totalFree() << "\n";
    }   
     int getAvailableSpaceForCategory(const string& category) {
        int totalFree = 0;
        for (Floor& f : floors) {
            for (Zone& z : f.getZones()) {
                if (z.isSuitableFor(category)) {
                    totalFree += z.totalFree();
                }
            }
        }
        return totalFree;
    }
    bool hasEnoughSpace(const string& category, int requiredUnits) {
        return getAvailableSpaceForCategory(category) >= requiredUnits;
    }
    void printSpaceSummary() {
        cout << "\n  === WAREHOUSE SPACE SUMMARY ===\n";
        map<string, int> categorySpace;
        
        for (Floor& f : floors) {
            for (Zone& z : f.getZones()) {
                string cat = z.getCategory();
                categorySpace[cat] += z.totalFree();
            }
        }
        cout << "  Category      | Free Slots\n";
        cout << "  " << string(30, '-') << "\n";
        for (const auto& entry : categorySpace) {
            cout << "  " << setw(12) << left << entry.first 
                 << " | " << entry.second << "\n";
        }
    }
};
//  DUMMY DATA
void loadDummyData(Warehouse& wh, Date today) {
    cout << "\n  Loading demo data...\n";
    // Orders
    auto* o1 = new InboundOrder("ORD-IN-001","SUP-Nvidia", 10, 850000,today);
    auto* o2 = new InboundOrder("ORD-IN-002","SUP-Intel",   5, 210000,today);
    auto* o3 = new InboundOrder("ORD-IN-003","SUP-Samsung",20,1900000,today);
    auto* o4 = new InboundOrder("ORD-IN-004","SUP-BattCo", 30, 240000,today);
    auto* o5 = new OutboundOrder("ORD-OUT-001","CUST-TechHub",2,200000,today);
    auto* o6 = new OutboundOrder("ORD-OUT-002","CUST-iStore", 1,110000,today);
    o1->setStatus('R'); o2->setStatus('R');
    o3->setStatus('P'); o4->setStatus('R');
    o5->setStatus('D'); o6->setStatus('P');
    for (auto* o : {(Order*)o1,(Order*)o2,(Order*)o3,
                    (Order*)o4,(Order*)o5,(Order*)o6})
        wh.dashboard.addOrder(o);

    // Crates
    Crate c1("NVGPU24","SUP-Nvidia","ORD-IN-001","GPU",        10,'G',"G001",today);
    Crate c2("INTCPU24","SUP-Intel","ORD-IN-002","CPU",          5,'G',"G001",today);
    Crate c3("SAMS24","SUP-Samsung","ORD-IN-003","phone",       20,'G',"G002",today);
    Crate c4("BATT24","SUP-BattCo","ORD-IN-004","Li-battery",  30,'G',"G003",today);
    Crate c5("DMGCPU","SUP-Intel","ORD-IN-002","CPU",           3,'D',"G001",today);

    wh.receiving.processCrate(c1);
    wh.receiving.processCrate(c2);
    wh.receiving.processCrate(c3);
    wh.receiving.processCrate(c4);
    wh.receiving.processCrate(c5);
    wh.receiving.flagDefect("SUP-Intel","CPU","Crate DMGCPU arrived crushed");
    wh.returnsQA.raiseDefect("SUP-Intel","CPU","3 units in DMGCPU damaged on arrival");

    // Assign stock
    for (StockItem& i : c1.generateItems(85000,24)) wh.assignLocation(i);
    for (StockItem& i : c2.generateItems(42000,36)) wh.assignLocation(i);
    for (StockItem& i : c3.generateItems(95000,12)) wh.assignLocation(i);
    for (StockItem& i : c4.generateItems(800,   6)) wh.assignLocation(i);

    // Sensor readings
    Zone* f4a = wh.findZone("F4-A");
    Zone* f2a = wh.findZone("F2-A");
    if (f4a) wh.envMonitor.pushReading(*f4a, 21.0, 32.0, false); // OK
    if (f2a) wh.envMonitor.pushReading(*f2a, 19.0, 43.0, false); // humidity breach

    // Security attempts
    wh.security.attemptEntry("G005","F4-A",9);  // vault guard — granted
    wh.security.attemptEntry("G001","F4-A",9);  // basic guard — denied
    wh.security.attemptEntry("S001","F4-A",9);  // supervisor  — granted
    wh.security.attemptEntry("G003","F2-A",10); // correct floor — granted

    cout << "  Demo data loaded — "
         << wh.inventory.totalItems() << " items in warehouse.\n";
}
//  MAIN MENU
int main() {
    Date      today(18, 3, 2026);
    Warehouse wh("JKUAT Electronics Warehouse", today);
    loadDummyData(wh, today);

    int choice = 0;
    while (choice != 11) {
        cout << "\n  ============================================\n";
        cout << "   JKUAT ELECTRONICS WAREHOUSE SYSTEM\n";
        cout << "  ============================================\n";
        cout << "  1.  Receive new crate\n";
        cout << "  2.  Place outbound order\n";
        cout << "  3.  Update order status\n";
        cout << "  4.  Zone entry attempt (Security)\n";
        cout << "  5.  Who is on duty?\n";
        cout << "  6.  Push sensor reading\n";
        cout << "  7.  View all sensor status\n";
        cout << "  8.  View floor layout\n";
        cout << "  9.  View daily dashboard\n";
        cout << "  10. View available space by category\n";
        cout << "  11. Exit\n";
        choice = readInt("  Choice: ", 1, 11);

        //  1. Receive new crate 
        if (choice == 1) {
            cout << "\n  -- Receive New Crate --\n";
            string crateId   = readString("  Crate serial number : ");
            string supplierId= readString("  Supplier ID         : ");
            string poId      = readString("  Purchase order ID   : ");
            string category  = pickCategory();
            int   units   = readInt("  Unit count (1-999)  : ", 1, 999);
            char  cond    = pickCondition();
            float price   = readPositiveFloat("  Unit price (KES)    : ");
            int   warranty= readInt("  Warranty (months)   : ", 1, 120);

            string staffId = pickStaff(wh.staffList);

            Crate crate(crateId, supplierId, poId, category,
                        units, cond, staffId, today);
            GRNReport grn = wh.receiving.processCrate(crate);
            grn.print();

                       if (cond != 'D') {
                cout << "\n  Assigning locations...\n";
                vector<StockItem> items = crate.generateItems(price, warranty);
                int assigned = 0;
                int failed = 0;
                for (StockItem& item : items) {
                    string loc = wh.assignLocation(item);
                    if (loc != "UNASSIGNED") {
                        assigned++;
                    } else {
                        failed++;
                    }
                }
                cout << "\n  ========================================\n";
                cout << "  ASSIGNMENT RESULT\n";
                cout << "  ========================================\n";
                cout << "  Total units:     " << units << "\n";
                cout << "  Successfully assigned: " << assigned << "\n";
                cout << "  FAILED to assign:     " << failed << "\n";
                if (failed > 0) {
                    cout << "\n  [!!!]  WARNING: SPACE SHORTAGE DETECTED! \n";
                    cout << "  [!!!] " << failed << " items could NOT be stored!\n";
                    cout << "  [!!!] These items remain in the receiving bay.\n";
                    cout << "  [!!!] Manual intervention required.\n";
                    // Raise a defect report for the shortage
                    DefectReport spaceAlert = wh.receiving.flagDefect(
                        supplierId, category, 
                        "SPACE SHORTAGE: " + to_string(failed) + " of " + to_string(units) + 
                        " units could not be stored. Only " + to_string(assigned) + " assigned.");
                    spaceAlert.print();
                    cout << "\n  [ACTION REQUIRED]\n";
                    cout << "  1. Check available space using option 10\n";
                    cout << "  2. Consider moving items to other zones\n";
                    cout << "  3. Contact warehouse manager\n";
                } else {
                    cout << "\n  [✓] All " << units << " items assigned successfully!\n";
                }
                
                cout << "\n  First serial number: " << crateId << "-001\n";
            } else {
                cout << "  [!] Crate rejected | raising defect report\n";
                DefectReport dr = wh.receiving.flagDefect(
                    supplierId, category, "Damaged on arrival");
                dr.print();
                wh.returnsQA.raiseDefect(supplierId, category,
                                         "Arrived in damaged crate");
            }
        }
        // 2. Place outbound order 
else if (choice == 2) {
    cout << "\n  -- Place Outbound Order --\n";
    string oid = readString("  Order ID    : ");
    string cid = readString("  Customer ID : ");

    // Build the order line by line — worker picks category + quantity
    struct OrderLine {
        string category;
        int    quantity;
    };
    vector<OrderLine> lines;
    float totalValue = 0;
    int   totalQty   = 0;

    bool addingItems = true;
    while (addingItems) {

        // Show a summary of what has been added so far
        if (!lines.empty()) {
            cout << "\n  Items in this order so far:\n";
            for (auto& l : lines)
                cout << "    " << setw(12) << left << l.category
                     << "  x" << l.quantity << "\n";
            cout << "  Running total: KES " << (int)totalValue << "\n";
        }

        // Check which categories actually have stock
        // Build a filtered list showing only categories with available items
        vector<string> availableCats;
        vector<int>    availableStock;
        for (const string& cat : CATEGORIES) {
            int free = wh.getAvailableSpaceForCategory(cat);
            // We need stock count not free space — add a helper call
            // For now we check if inventory has this category
            // Count items in inventory matching category
            int inStock = 0;
            for (const StockItem& s : wh.inventory.getStock()) {
                if (s.getCategory() == cat && s.getCondition() == 'G')
                    inStock++;
            }
            if (inStock > 0) {
                availableCats.push_back(cat);
                availableStock.push_back(inStock);
            }
        }
        if (availableCats.empty()) {
            cout << "\n  [!] No stock available to dispatch.\n";
            break;
        }
        // Show available categories with stock counts
        cout << "\n  Available stock:\n";
        for (int i = 0; i < (int)availableCats.size(); i++)
            cout << "  " << (i + 1) << ". "
                 << setw(14) << left << availableCats[i]
                 << "  (" << availableStock[i] << " units available)\n";
        cout << "  " << (availableCats.size() + 1) << ". Done finish order\n";

        int pick = readInt("  Select item: ", 1,
                           (int)availableCats.size() + 1);
        if (pick == (int)availableCats.size() + 1) {
            // Worker is done adding lines
            if (lines.empty()) {
                cout << "  [!] Cannot place an empty order.\n";
                addingItems = false;
            } else {
                addingItems = false;
            }
        } else {
            string chosenCat  = availableCats[pick - 1];
            int    maxAllowed = availableStock[pick - 1];

            // Check if this category is already in the order
            bool alreadyAdded = false;
            for (auto& l : lines) {
                if (l.category == chosenCat) {
                    alreadyAdded = true;
                    cout << "  [!] " << chosenCat
                         << " already added. Edit quantity instead.\n";

                    int subChoice = pickOption(
                        "What would you like to do?",
                        {"Update quantity",
                         "Remove this line",
                         "Cancel  go back"});

                    if (subChoice == 1) {
                        // Show how many are already in the order
                        cout << "  Currently: " << l.quantity << " units\n";
                        int newQty = readInt(
                            "  New quantity (1-" + to_string(maxAllowed) + "): ",
                            1, maxAllowed);
                        // Adjust running total
                        // Find average unit price for this category from stock
                        float unitPrice = 0;
                        int   priceCount = 0;
                        for (const StockItem& s : wh.inventory.getStock()) {
                            if (s.getCategory() == chosenCat) {
                                unitPrice += s.getPrice();
                                priceCount++;
                            }
                        }
                        if (priceCount > 0) unitPrice /= priceCount;
                        totalValue -= l.quantity * unitPrice;
                        totalValue += newQty    * unitPrice;
                        totalQty   -= l.quantity;
                        totalQty   += newQty;
                        l.quantity  = newQty;
                        cout << "  [OK] Updated " << chosenCat
                             << " quantity to " << newQty << "\n";
                    } else if (subChoice == 2) {
                        // Remove this line
                        float unitPrice = 0;
                        int   priceCount = 0;
                        for (const StockItem& s : wh.inventory.getStock()) {
                            if (s.getCategory() == chosenCat) {
                                unitPrice += s.getPrice();
                                priceCount++;
                            }
                        }
                        if (priceCount > 0) unitPrice /= priceCount;
                        totalValue -= l.quantity * unitPrice;
                        totalQty   -= l.quantity;
                        lines.erase(
                            remove_if(lines.begin(), lines.end(),
                                [&](const OrderLine& ol) {
                                    return ol.category == chosenCat;
                                }),
                            lines.end());
                        cout << "  [OK] Removed " << chosenCat
                             << " from order.\n";
                    }
                    // subChoice 3 = just go back
                    break;
                }
            }
            if (!alreadyAdded) {
                int qty = readInt(
                    "  Quantity (1-" + to_string(maxAllowed) + "): ",
                    1, maxAllowed);
                // Calculate value from average stock price for this category
                float unitPrice  = 0;
                int   priceCount = 0;
                for (const StockItem& s : wh.inventory.getStock()) {
                    if (s.getCategory() == chosenCat) {
                        unitPrice += s.getPrice();
                        priceCount++;
                    }
                }
                if (priceCount > 0) unitPrice /= priceCount;
                float lineValue = unitPrice * qty;
                totalValue += lineValue;
                totalQty   += qty;
                lines.push_back({chosenCat, qty});

                cout << "  [OK] Added " << qty << " x " << chosenCat
                     << "    KES " << (int)lineValue
                     << "  (@ KES " << (int)unitPrice << " each)\n";
            }
        }
    }
    // Confirm and place if lines exist
    if (!lines.empty()) {
        cout << "\n  ========================================\n";
        cout << "  ORDER SUMMARY — " << oid << "\n";
        cout << "  Customer: " << cid << "\n";
        cout << "  ========================================\n";
        for (auto& l : lines)
            cout << "    " << setw(14) << left << l.category
                 << "  x" << l.quantity << "\n";
        cout << "  ----------------------------------------\n";
        cout << "  Total items  : " << totalQty   << "\n";
        cout << "  Total value  : KES " << (int)totalValue << "\n";
        cout << "  ========================================\n";
        int confirm = pickOption("Confirm order?",
            {"Yes place order",
             "No  discard"});
        if (confirm == 1) {
            OutboundOrder* o = wh.dispatch.createOrder(
                oid, cid, totalQty, totalValue);
            wh.dashboard.addOrder(o);

            // Mark items as reserved in inventory
            for (auto& l : lines) {
                int reserved = 0;
                for (StockItem& s : wh.inventory.getStockMutable()) {
                    if (s.getCategory() == l.category &&
                        s.getCondition() == 'G'        &&
                        reserved < l.quantity) {
                        s.setCondition('R'); // 'R' = reserved
                        reserved++;
                    }
                }
            }
            cout << "\n  [OK] Order " << oid
                 << " placed — " << totalQty
                 << " items reserved for dispatch.\n";
        } else {
            cout << "  [OK] Order discarded.\n";
        }
    }
}
        //  3. Update order status 
        else if (choice == 3) {
            cout << "\n  -- Update Order Status --\n";
            if (wh.dashboard.getOrders().empty()) {
                cout << "  [!] No orders in the system yet.\n";
            } else {
                cout << "\n  Current orders:\n";
                int idx = 1;
                for (Order* o : wh.dashboard.getOrders()) {
                    cout << "  " << idx++ << ". ";
                    o->print();
                }
                int pick = readInt("  Select order number: ", 1,
                                   (int)wh.dashboard.getOrders().size());
                Order* chosen = wh.dashboard.getOrders()[pick - 1];
                cout << "  Current status: " << chosen->statusLabel() << "\n";
                char ns = pickOrderStatus();
                chosen->setStatus(ns);
                cout << "  [OK] Updated to: " << chosen->statusLabel() << "\n";
            }
        }
        // 4. Zone entry attempt 
        else if (choice == 4) {
            cout << "\n  -- Zone Entry Attempt --\n";
            string staffId = pickStaff(wh.staffList);
            string zone    = pickZone();
            int    hour    = readInt("  Hour (0-23)  : ", 0, 23);

            AccessLog log = wh.security.attemptEntry(staffId, zone, hour);
            log.print();
            cout << "  Total denied today: "
                 << wh.security.totalDenied() << "\n";
        }
        //  5. Who is on duty 
        else if (choice == 5) {
            int hour = readInt("\n  Enter hour (0-23): ", 0, 23);
            wh.schedule.printOnDuty(hour);
        }
        // 6. Push sensor reading 
        else if (choice == 6) {
            cout << "\n  -- Push Sensor Reading --\n";
            string zoneId = pickZone();
            Zone*  z      = wh.findZone(zoneId);
            if (!z) { cout << "  [!] Zone not found\n"; }
            else {
                float temp = readPositiveFloat("  Temperature (C) : ");
                float hum  = readPositiveFloat("  Humidity (%)    : ");
                int   smoke= pickOption("Smoke detected?",
                                 {"No : no smoke", "Yes : smoke detected!"}) - 1;
                wh.envMonitor.pushReading(*z, temp, hum, smoke == 1);
                cout << z->sensor.statusLine() << "\n";
                cout << "  Total alerts today: "
                     << wh.envMonitor.totalAlerts() << "\n";
            }
        }
        //  7. All sensor status
        else if (choice == 7) {
            wh.envMonitor.printAllSensors(wh.floors);
        }
        //  8. Floor layout 
        else if (choice == 8) {
            int sub = pickOption("View options:",
                {"All floors : full layout",
                 "Single floor : choose which",
                 "Full inventory list"});
            if (sub == 1) {
                wh.printManagerView();
            } else if (sub == 2) {
                int fn = pickFloor();
                wh.floors[fn-1].printStatus();
                cout << "  Free slots on Floor " << fn << ": "
                     << wh.floors[fn-1].totalFree() << "\n";
            } else {
                wh.inventory.printSummary();
            }
        }
        // 9. Daily dashboard 
        else if (choice == 9) {
            wh.dashboard.print();
        }
        // 10. Space summary
          else if (choice == 10) {
            wh.printSpaceSummary();
        }
    }

    cout << "\n  System closed. Goodbye.\n";
    return 0;
}