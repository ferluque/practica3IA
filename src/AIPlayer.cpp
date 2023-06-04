#include "AIPlayer.h"
#include "Parchis.h"

int hijosExplorados = 0;
const double masinf = 9999999999.0, menosinf = -9999999999.0;
const double gana = masinf - 1, pierde = menosinf + 1;
const int num_pieces = 3;
const int PROFUNDIDAD_MINIMAX = 4;  // Umbral maximo de profundidad para el metodo MiniMax
const int PROFUNDIDAD_ALFABETA = 6; // Umbral maximo de profundidad para la poda Alfa_Beta

bool AIPlayer::move()
{
    cout << "Realizo un movimiento automatico" << endl;

    color c_piece;
    int id_piece;
    int dice;
    think(c_piece, id_piece, dice);

    cout << "Movimiento elegido: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    actual->movePiece(c_piece, id_piece, dice);
    return true;
}

void AIPlayer::think(color &c_piece, int &id_piece, int &dice) const
{
    clock_t inicio = clock();
    switch (id)
    {
    case 0:
        cout << "PODA ALFA BETA VALORACIONTEST" << endl;
        AlfaBeta(c_piece, id_piece, dice, ValoracionTest);
        cout << "=================================================" << endl;
        cout << "Tiempo invertido: " << (double)(clock() - inicio) / CLOCKS_PER_SEC << endl;
        cout << "Hijos explorados: " << hijosExplorados << endl;
        cout << "=================================================" << endl;
        hijosExplorados = 0;
        break;
    case 1:
        cout << "PODA ALFA BETA MIHEURISTICA" << endl;
        AlfaBeta(c_piece, id_piece, dice, MiHeuristica);
        cout << "Tiempo invertido: " << (double)(clock() - inicio) / CLOCKS_PER_SEC << endl;
        cout << "Hijos explorados: " << hijosExplorados << endl;
        cout << "=================================================" << endl;
        hijosExplorados = 0;
        break;
    }
}

// =================================================
// ALFABETA =========================================
// =================================================
bool print = true;
double AlfaBetaRecursivo(const Parchis &current, const int &profundidad, const int &Max_Player, const double &alpha, const double &beta, double (*heuristic)(const Parchis &, int))
{
    double resultado;
    if (profundidad == PROFUNDIDAD_ALFABETA)
    {
        resultado = heuristic(current, Max_Player);
        // if (print) cout << string(profundidad,'\t') << "Vk: " << resultado << endl;
        return resultado;
    }
    else
    {
        bool isMax = current.getCurrentPlayerId() == Max_Player;
        double currentAlpha = alpha, currentBeta = beta;
        ParchisBros hijos = current.getChildren();
        int i = 0;
        // if (print) cout << string(profundidad, '\t') << "Nodo actual: " << ((isMax) ? "MAX" : "MIN") << " alfa beta: " << alpha << " " << beta << endl;
        for (ParchisBros::Iterator it = hijos.begin(); it != hijos.end(); ++it)
        {
            double Vk = AlfaBetaRecursivo(*it, profundidad + 1, Max_Player, currentAlpha, currentBeta, heuristic);

            if (isMax && (Vk > currentAlpha))
                currentAlpha = Vk;
            if (!isMax && (Vk < currentBeta))
                currentBeta = Vk;

            if (currentAlpha >= currentBeta)
                break;
        }
        if (isMax)
            return currentAlpha;
        else
            return currentBeta;
    }
}

void AIPlayer::AlfaBeta(color &c_piece, int &id_piece, int &dice, double (*heuristic)(const Parchis &, int)) const
{
    double alpha = pierde, beta = gana;
    ParchisBros hijos = actual->getChildren();
    int Max_Player = actual->getCurrentPlayerId();
    double max;
    int i = 0;
    // cout << "MAX_PLAYER: " << Max_Player << endl;
    // cout << "Nodo actual: MAX " << alpha << " " << beta << endl;
    for (ParchisBros::Iterator it = hijos.begin(); it != hijos.end(); ++it)
    {
        // cout << "Analizando hijo " << i << ". Movimiento elegido: " << str(it.getMovedColor()) << " " << it.getMovedPieceId() << " " << it.getMovedDiceValue() << endl;
        clock_t inicio = clock();
        double Vk = AlfaBetaRecursivo(*it, 1, Max_Player, alpha, beta, heuristic);
        // cout << "Tiempo en hijo: " << i << ": " << (double)(clock()-inicio)/CLOCKS_PER_SEC << endl;
        if (Vk > alpha)
            alpha = Vk;
        bool print = true;
        // if (print) cout << "Vk hijo: " << i << ": " << Vk << endl;
        // if (print) cout << "Alpha Beta " << alpha << " " << beta << endl;
        // if (print) cout << "================================================" << endl;
        ++hijosExplorados;
        if (i == 0)
        {
            // cout << "Actualizo" << endl;
            max = Vk;
            c_piece = it.getMovedColor();
            id_piece = it.getMovedPieceId();
            dice = it.getMovedDiceValue();
        }
        else
        {
            if (Vk > max)
            {
                // cout << "Actualizo" << endl;
                max = Vk;
                c_piece = it.getMovedColor();
                id_piece = it.getMovedPieceId();
                dice = it.getMovedDiceValue();
            }
        }
        ++i;
    }
    cout << "Valor de heurística encontrado por elección: " << max << endl;
}

// =================================================
// TUTORIAL ========================================
// =================================================
void AIPlayer::thinkAleatorio(color &c_piece, int &id_piece, int &dice) const
{
    // IMPLEMENTACIÓN INICIAL DEL AGENTE
    // Esta implementación realiza un movimiento aleatorio.
    // Se proporciona como ejemplo, pero se debe cambiar por una que realice un movimiento inteligente
    // como lo que se muestran al final de la función.

    // OBJETIVO: Asignar a las variables c_piece, id_piece, dice (pasadas por referencia) los valores,
    // respectivamente, de:
    // - color de ficha a mover
    // - identificador de la ficha que se va a mover
    // - valor del dado con el que se va a mover la ficha.

    // El id de mi jugador actual.
    int player = actual->getCurrentPlayerId();

    // Vector que almacenará los dados que se pueden usar para el movimiento
    vector<int> current_dices;
    // Vector que almacenará los ids de las fichas que se pueden mover para el dado elegido.
    vector<tuple<color, int>> current_pieces;

    // Se obtiene el vector de dados que se pueden usar para el movimiento
    current_dices = actual->getAvailableNormalDices(player);
    // Elijo un dado de forma aleatoria.
    dice = current_dices[rand() % current_dices.size()];

    // Se obtiene el vector de fichas que se pueden mover para el dado elegido
    current_pieces = actual->getAvailablePieces(player, dice);

    // Si tengo fichas para el dado elegido muevo una al azar.
    if (current_pieces.size() > 0)
    {
        int random_id = rand() % current_pieces.size();
        id_piece = get<1>(current_pieces[random_id]); // get<i>(tuple<...>) me devuelve el i-ésimo
        c_piece = get<0>(current_pieces[random_id]);  // elemento de la tupla
    }
    else
    {
        // Si no tengo fichas para el dado elegido, pasa turno (la macro SKIP_TURN me permite no mover).
        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.
    }
}

void AIPlayer::thinkAleatorioMasInteligente(color &c_piece, int &id_piece, int &dice) const
{
    // El número de mi jugador actual
    int player = actual->getCurrentPlayerId();

    // Vector que almacenará los dados que se pueden usar para el movimiento
    vector<int> current_dices_con_especiales;
    // Ids de las fichas que se pueden mover
    vector<tuple<color, int>> current_pieces;

    // Acceder a todos los dados disponible
    current_dices_con_especiales = actual->getAllAvailableDices(player);

    // Miramos que dados pueden mover ficha
    vector<int> current_dices_que_pueden_mover_ficha;
    for (int i = 0; i < current_dices_con_especiales.size(); ++i)
    {
        current_pieces = actual->getAvailablePieces(player, current_dices_con_especiales[i]);
        if (current_pieces.size() > 0)
            current_dices_que_pueden_mover_ficha.push_back(current_dices_con_especiales[i]);
    }
    // Si ninguno puede, pasamos turno con un dado al azar
    if (current_dices_que_pueden_mover_ficha.size() == 0)
    {
        dice = current_dices_con_especiales[rand() % current_dices_con_especiales.size()];
        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor();
    }
    // Si hay alguno que puede mover lo escogemos aleatoriamente
    else
    {
        dice = current_dices_que_pueden_mover_ficha[rand() % current_dices_que_pueden_mover_ficha.size()];
        current_pieces = actual->getAvailablePieces(player, dice);
        // Movemos una al azar de las que se pueden mover para el dado indicado
        int random_id = rand() % current_pieces.size();
        c_piece = get<0>(current_pieces[random_id]);
        id_piece = get<1>(current_pieces[random_id]);
    }
}

void AIPlayer::thinkFichaMasAdelantada(color &c_piece, int &id_piece, int &dice) const
{
    thinkAleatorioMasInteligente(c_piece, id_piece, dice);

    int player = actual->getCurrentPlayerId();
    vector<tuple<color, int>> current_pieces = actual->getAvailablePieces(player, dice);

    int id_ficha_mas_adelantada = -1;
    color col_ficha_mas_adelantada = none;
    int min_distancia_meta = 9999;
    for (int i = 0; i < current_pieces.size(); ++i)
    {
        color col = get<0>(current_pieces[i]);
        int id = get<1>(current_pieces[i]);
        int distancia_meta = actual->distanceToGoal(col, id);
        if (distancia_meta < min_distancia_meta)
        {
            min_distancia_meta = distancia_meta;
            id_ficha_mas_adelantada = id;
            col_ficha_mas_adelantada = col;
        }
    }

    if (id_ficha_mas_adelantada == -1)
    {
        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor();
    }
    else
    {
        id_piece = id_ficha_mas_adelantada;
        c_piece = col_ficha_mas_adelantada;
    }
}

void AIPlayer::thinkMejorOpcion(color &c_piece, int &id_piece, int &dice) const
{
    // Queremos acceder a todos los hijos
    ParchisBros hijos = actual->getChildren();
    bool me_quedo_con_esta_accion = false;
    // Iteramos en los hijos
    for (ParchisBros::Iterator it = hijos.begin(); it != hijos.end() and !me_quedo_con_esta_accion; ++it)
    {
        Parchis siguiente_hijo = *it;
        if (siguiente_hijo.isEatingMove() or siguiente_hijo.isGoalMove() or (siguiente_hijo.gameOver() and siguiente_hijo.getWinner() == this->jugador))
        {
            me_quedo_con_esta_accion = true;
            c_piece = it.getMovedColor();
            id_piece = it.getMovedPieceId();
            dice = it.getMovedDiceValue();
        }
        if (!me_quedo_con_esta_accion)
            thinkFichaMasAdelantada(c_piece, id_piece, dice);
    }
}

double AIPlayer::ValoracionTest(const Parchis &estado, int jugador)
{
    // Heurística de prueba proporcionada para validar el funcionamiento del algoritmo de búsqueda.

    int ganador = estado.getWinner();
    int oponente = (jugador + 1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador)
    {
        return gana;
    }
    else if (ganador == oponente)
    {
        return pierde;
    }
    else
    {
        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);

        // Recorro todas las fichas de mi jugador
        int puntuacion_jugador = 0;
        // Recorro colores de mi jugador.
        for (int i = 0; i < my_colors.size(); i++)
        {
            color c = my_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                // Valoro positivamente que la ficha esté en casilla segura o meta.
                if (estado.isSafePiece(c, j))
                {
                    puntuacion_jugador++;
                }
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                {
                    puntuacion_jugador += 5;
                }
            }
        }

        // Recorro todas las fichas del oponente
        int puntuacion_oponente = 0;
        // Recorro colores del oponente.
        for (int i = 0; i < op_colors.size(); i++)
        {
            color c = op_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                if (estado.isSafePiece(c, j))
                {
                    // Valoro negativamente que la ficha esté en casilla segura o meta.
                    puntuacion_oponente++;
                }
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                {
                    puntuacion_oponente += 5;
                }
            }
        }

        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return puntuacion_jugador - puntuacion_oponente;
    }
}

bool estaSegura(const Parchis &estado, int jugadorActual, const color &c, const int p)
{
    bool segura = true;
    vector<color> opColors = estado.getPlayerColors((jugadorActual + 1) % 2);
    for (int i = 0; i < opColors.size() && segura; i++)
    {
        for (int j = 0; j < num_pieces && segura; j++)
        {
            int dist = estado.distanceBoxtoBox(opColors[i], j, c, p);
            vector<int> oponentDices = estado.getAvailableNormalDices((jugadorActual + 1) % 2);
            auto dado = find(oponentDices.begin(), oponentDices.end(), dist);
            if (dado != oponentDices.end())
            {
                segura = 
                (estado.isSafePiece(c, p) && estado.getBoard().getPiece(opColors[i], j).get_type()==star_piece) // solo puede comernos si es estrella estando en una casilla segura
                or (!estado.isSafePiece(c, p) && estado.getBoard().getPiece(c,p).get_type()==star_piece);
                // if (!segura)
                //     cout << "No es segura porque " << str(opColors[i]) << " en " << estado.getBoard().getPiece(opColors[i], j).get_box().num << " se puede comer a " << str(c) << " en " << estado.getBoard().getPiece(c, p).get_box().num << " moviendo " << *dado << endl;
            }
        }
    }
    return segura;
}

double AIPlayer::MiHeuristica(const Parchis &estado, int jugador)
{
    int ganador = estado.getWinner();
    int oponente = (jugador + 1) % 2;
    vector<double> puntuaciones(2);
    // Puntuaciones[0] es actual y puntuaciones[1] es oponente
    puntuaciones[0] = puntuaciones[1] = 0.0;
    if (ganador == jugador)
        return gana;
    else if (ganador == oponente)
        return pierde;
    // Colores que juega mi jugador y colores del oponente
    vector<color> my_colors = estado.getPlayerColors(jugador);
    vector<color> op_colors = estado.getPlayerColors(oponente);

    // if (estado.isEatingMove())
    //     puntuaciones[jugador]+=50;
    // Recorro fichas jugador y oponente
    for (int jugadorActual = 0; jugadorActual < 2; jugadorActual++)
    {
        int betterValue = static_cast<int>(pierde);
        for (int i = 0; i < my_colors.size(); i++)
        {
            color c = my_colors[i];
            // Valoro que haya fichas en el objetivo
            // puntuaciones[jugadorActual] += 10 * estado.piecesAtGoal(c);
            // Valoro que las fichas estén seguras
            int cont = 0;
            for (int j = 0; j < num_pieces; ++j)
            {
                if (estaSegura(estado, (jugadorActual + jugador) % 2, c, j))
                // if (estado.isSafePiece(c, j))
                {
                    puntuaciones[jugadorActual] += (74 - estado.distanceToGoal(c, j));
                }
                else
                {
                    puntuaciones[jugadorActual] -= (74 - estado.distanceToGoal(c, j));
                }
            }

            int acumulacion = 0;
            // Heurística = min(3*72-avg(distanciasColor1), 3*72-avg(distanciaColor2))
            for (int j = 0; j < num_pieces; j++)
            {
                int dist = estado.distanceToGoal(c, j);
                int val = 3 * (74 - dist);
                acumulacion += val;
            }
            puntuaciones[jugadorActual] += acumulacion / num_pieces;
            // acumulacion = 0;
            // if (acumulacion>betterValue) {
            //     betterValue = acumulacion;
            // //     // cout << "betterValue:" << betterValue << endl;
            // }
            acumulacion = 0;
        }
        // puntuaciones[jugadorActual] += betterValue;

        swap(my_colors, op_colors);
    }
    return puntuaciones[0] - puntuaciones[1];
}