#include <benchmark/benchmark.h>

#include <vector>
#include <string>
#include <unordered_map>

#define NAME "Design Day Name"
#define SELECTED "All"

// DATE
#define DAYOFMONTH "Day Of Month"
#define MONTH "Month"
#define DAYTYPE "Day Type"
#define DAYLIGHTSAVINGTIMEINDICATOR "Daylight Saving Time Indicator"

// TEMPERATURE
#define MAXIMUMDRYBULBTEMPERATURE "Maximum Dry Bulb Temperature"
#define DAILYDRYBULBTEMPERATURERANGE "Daily Dry Bulb Temperature Range"
#define DAILYWETBULBTEMPERATURERANGE "Daily Wet Bulb Temperature Range"
#define DRYBULBTEMPERATURERANGEMODIFIERTYPE "Dry Bulb Temperature Range Modifier Type"
#define DRYBULBTEMPERATURERANGEMODIFIERSCHEDULE "Dry Bulb Temperature Range Modifier Schedule"

// HUMIDITY
#define HUMIDITYINDICATINGCONDITIONSATMAXIMUMDRYBULB "Humidity Indicating Conditions At Maximum Dry Bulb"
#define HUMIDITYINDICATINGTYPE "Humidity Indicating Type"
#define HUMIDITYINDICATINGDAYSCHEDULE "Humidity Indicating Day Schedule"

// PRESSURE / WIND / PRECIP
#define BAROMETRICPRESSURE "Barometric Pressure"
#define WINDSPEED "Wind Speed"
#define WINDDIRECTION "Wind Direction"
#define RAININDICATOR "Rain Indicator"
#define SNOWINDICATOR "Snow Indicator"

// SOLAR
#define SOLARMODELINDICATOR "Solar Model Indicator"
#define BEAMSOLARDAYSCHEDULE "Beam Solar Day Schedule"
#define DIFFUSESOLARDAYSCHEDULE "Diffuse Solar Day Schedule"
#define ASHRAETAUB "ASHRAE Taub"
#define ASHRAETAUD "ASHRAE Taud"
#define SKYCLEARNESS "Sky Clearness"


namespace current {
  class OSGridController {
    public:
      OSGridController() {};

      void addCategoryAndFields(const std::pair<std::string, std::vector<std::string>>& categoryAndFields) {
        m_categoriesAndFields.push_back(categoryAndFields);
      }

    private:
      std::vector<std::pair<std::string, std::vector<std::string> > > m_categoriesAndFields;
  };
}

namespace proposed {
  class OSGridController {
    public:
      OSGridController() {};

      void addCategoryAndFields(std::pair<std::string, std::vector<std::string>>&& categoryAndFields) {
        m_categoriesAndFields.emplace_back(categoryAndFields);
      }

    private:
      std::vector<std::pair<std::string, std::vector<std::string> > > m_categoriesAndFields;
  };
}

namespace proposedonego {
  class OSGridController {
    public:
      OSGridController() {};

      void setCategoryAndFields(std::vector<std::pair<std::string, std::vector<std::string> > >& categoryAndFields) {
        m_categoriesAndFields = std::move(categoryAndFields);
      }

    private:
      std::vector<std::pair<std::string, std::vector<std::string> > > m_categoriesAndFields;
  };
}

namespace proposedmap {
  class OSGridController {
    public:
      OSGridController() {};

      void setCategoryAndFields(std::unordered_map<std::string, std::vector<std::string> >& categoryAndFields) {
        m_categoriesAndFields = std::move(categoryAndFields);
      }

    private:
      std::unordered_map<std::string, std::vector<std::string> > m_categoriesAndFields;
  };
}


static void Current(benchmark::State& state) {

  current::OSGridController grid;

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {

    {
      std::vector<std::string> fields;
      fields.push_back(DAYOFMONTH);
      fields.push_back(MONTH);
      fields.push_back(DAYTYPE);
      fields.push_back(DAYLIGHTSAVINGTIMEINDICATOR);
      std::pair<std::string, std::vector<std::string>> categoryAndFields = std::make_pair("Date", fields);
      grid.addCategoryAndFields(categoryAndFields);
    }

    {
      std::vector<std::string> fields;
      fields.push_back(MAXIMUMDRYBULBTEMPERATURE);
      fields.push_back(DAILYDRYBULBTEMPERATURERANGE);
      fields.push_back(DAILYWETBULBTEMPERATURERANGE);
      fields.push_back(DRYBULBTEMPERATURERANGEMODIFIERTYPE);
      fields.push_back(DRYBULBTEMPERATURERANGEMODIFIERSCHEDULE);
      std::pair<std::string, std::vector<std::string>> categoryAndFields = std::make_pair("Temperature", fields);
      grid.addCategoryAndFields(categoryAndFields);
    }

    {
      std::vector<std::string> fields;
      fields.push_back(HUMIDITYINDICATINGCONDITIONSATMAXIMUMDRYBULB);
      fields.push_back(HUMIDITYINDICATINGTYPE);
      fields.push_back(HUMIDITYINDICATINGDAYSCHEDULE);
      std::pair<std::string, std::vector<std::string>> categoryAndFields = std::make_pair("Humidity", fields);
      grid.addCategoryAndFields(categoryAndFields);
    }

    {
      std::vector<std::string> fields;
      fields.push_back(BAROMETRICPRESSURE);
      fields.push_back(WINDSPEED);
      fields.push_back(WINDDIRECTION);
      fields.push_back(RAININDICATOR);
      fields.push_back(SNOWINDICATOR);
      std::pair<std::string, std::vector<std::string>> categoryAndFields = std::make_pair("Pressure\nWind\nPrecipitation", fields);
      grid.addCategoryAndFields(categoryAndFields);
    }

    {
      std::vector<std::string> fields;
      fields.push_back(SOLARMODELINDICATOR);
      fields.push_back(BEAMSOLARDAYSCHEDULE);
      fields.push_back(DIFFUSESOLARDAYSCHEDULE);
      fields.push_back(ASHRAETAUB);
      fields.push_back(ASHRAETAUD);
      fields.push_back(SKYCLEARNESS);
      std::pair<std::string, std::vector<std::string>> categoryAndFields = std::make_pair("Solar", fields);
      grid.addCategoryAndFields(categoryAndFields);
    }

  }
}

static void Proposed(benchmark::State& state) {

  proposed::OSGridController grid;

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {
    {
      std::vector<std::string> fields{DAYOFMONTH, MONTH, DAYTYPE};
      grid.addCategoryAndFields(std::make_pair("Date", fields));
    }


    {
      std::vector<std::string> fields {
          MAXIMUMDRYBULBTEMPERATURE,
          DAILYDRYBULBTEMPERATURERANGE,
          DAILYWETBULBTEMPERATURERANGE,
          DRYBULBTEMPERATURERANGEMODIFIERTYPE,
          DRYBULBTEMPERATURERANGEMODIFIERSCHEDULE
      };
      grid.addCategoryAndFields(std::make_pair("Temperature", fields));
    }

    {
      std::vector<std::string> fields {
          HUMIDITYINDICATINGCONDITIONSATMAXIMUMDRYBULB,
          HUMIDITYINDICATINGTYPE,
          HUMIDITYINDICATINGDAYSCHEDULE
      };
      grid.addCategoryAndFields(std::make_pair("Humidity", fields));
    }

    {
      std::vector<std::string> fields {
          BAROMETRICPRESSURE,
          WINDSPEED,
          WINDDIRECTION,
          RAININDICATOR,
          SNOWINDICATOR
      };
      grid.addCategoryAndFields(std::make_pair("Pressure\nWind\nPrecipitation", fields));
    }

    {
      std::vector<std::string> fields {
          SOLARMODELINDICATOR,
          BEAMSOLARDAYSCHEDULE,
          DIFFUSESOLARDAYSCHEDULE,
          ASHRAETAUB,
          ASHRAETAUD,
          SKYCLEARNESS
      };
      grid.addCategoryAndFields(std::make_pair("Solar", fields));
    }

  }
}

static void ProposedOneGo(benchmark::State& state) {

  proposedonego::OSGridController grid;

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {

    std::vector<std::pair<std::string, std::vector<std::string> > > x {
      {"Date", {DAYOFMONTH, MONTH, DAYTYPE}},
      {"Temperature", {
          MAXIMUMDRYBULBTEMPERATURE,
          DAILYDRYBULBTEMPERATURERANGE,
          DAILYWETBULBTEMPERATURERANGE,
          DRYBULBTEMPERATURERANGEMODIFIERTYPE,
          DRYBULBTEMPERATURERANGEMODIFIERSCHEDULE}},
      {"Humidity", {
          HUMIDITYINDICATINGCONDITIONSATMAXIMUMDRYBULB,
          HUMIDITYINDICATINGTYPE,
          HUMIDITYINDICATINGDAYSCHEDULE}},
      {"Pressure\nWind\nPrecipitation", {
          BAROMETRICPRESSURE,
          WINDSPEED,
          WINDDIRECTION,
          RAININDICATOR,
          SNOWINDICATOR}},
      {"Solar", {
          SOLARMODELINDICATOR,
          BEAMSOLARDAYSCHEDULE,
          DIFFUSESOLARDAYSCHEDULE,
          ASHRAETAUB,
          ASHRAETAUD,
          SKYCLEARNESS}}
    };
    grid.setCategoryAndFields(x);

  }
}

static void ProposedMap(benchmark::State& state) {

  proposedmap::OSGridController grid;

  // Code inside this loop is measured repeatedly
  for (auto _ : state) {

    std::unordered_map<std::string, std::vector<std::string> > x {
      {"Date", {DAYOFMONTH, MONTH, DAYTYPE}},
      {"Temperature", {
          MAXIMUMDRYBULBTEMPERATURE,
          DAILYDRYBULBTEMPERATURERANGE,
          DAILYWETBULBTEMPERATURERANGE,
          DRYBULBTEMPERATURERANGEMODIFIERTYPE,
          DRYBULBTEMPERATURERANGEMODIFIERSCHEDULE}},
      {"Humidity", {
          HUMIDITYINDICATINGCONDITIONSATMAXIMUMDRYBULB,
          HUMIDITYINDICATINGTYPE,
          HUMIDITYINDICATINGDAYSCHEDULE}},
      {"Pressure\nWind\nPrecipitation", {
          BAROMETRICPRESSURE,
          WINDSPEED,
          WINDDIRECTION,
          RAININDICATOR,
          SNOWINDICATOR}},
      {"Solar", {
          SOLARMODELINDICATOR,
          BEAMSOLARDAYSCHEDULE,
          DIFFUSESOLARDAYSCHEDULE,
          ASHRAETAUB,
          ASHRAETAUD,
          SKYCLEARNESS}}
    };
    grid.setCategoryAndFields(x);

  }
}

BENCHMARK(Current);
BENCHMARK(Proposed);
BENCHMARK(ProposedOneGo);
BENCHMARK(ProposedMap);

