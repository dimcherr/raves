#include "data/dcue.h"
#include "data/devent.h"

namespace acue {

Cue firstRoom {{
    {{
        "Where did that music come from? It's so ethereal.",
        "Откуда эта музыка? Она такая неземная.",
    }, true, acue::Speaker::me},
    {{
        "I need to find its source.",
        "Мне нужно найти её источник.",
    }, true, acue::Speaker::me},
}};

Cue firstRoomExit {{
    {{
        "I can't believe this workshop.",
        "Мастерская просто невероятна.",
    }, true, acue::Speaker::me},
    {{
        "Are they really making orchestrins here? It's so surreal.",
        "Они правда делают здесь оркестрины? Очень причудливо.",
    }, true, acue::Speaker::me},
}};

Cue firstHallway {{
    {{
        "The music boxes I found here are astonishing.",
        "Музыкальные шкатулки, которые я нахожу здесь поразительны.",
    }, true, acue::Speaker::me},
    {{
        "The figurines feel almost alive.",
        "Фигурки кажутся почти живыми.",
    }, true, acue::Speaker::me},
}};

Cue secondHallway {{
    {{
        "I feel like I'm really close...",
        "У меня такое чувство, что я очень близко к месту,",
    }, true, acue::Speaker::me},
    {{
        "...to hearing that hypnotizing melody again.",
        "откуда доносилась эта гипнотизирующая мелодия.",
    }, true, acue::Speaker::me},
}};

}