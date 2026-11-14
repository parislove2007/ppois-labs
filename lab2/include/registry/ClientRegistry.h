#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "people/Client.h"
#include "value/PhoneNumber.h"

namespace repair
{

/**
 * @brief Реестр клиентов сервисного центра.
 *
 * Ключом служит телефон: по нему клиента ищут на приёмке. Клиенты
 * хранятся умными указателями, чтобы корпоративные не теряли свой тип.
 */
class ClientRegistry
{
public:
    /** @brief Создаёт пустой реестр. */
    ClientRegistry();

    /** @return Количество зарегистрированных клиентов. */
    std::size_t size() const;

    /**
     * @brief Регистрирует клиента.
     * @param client Регистрируемый клиент.
     * @throws ValidationException Если клиент не задан.
     * @throws BusinessRuleException Если телефон уже занят.
     */
    void registerClient(const std::shared_ptr<Client>& client);

    /**
     * @brief Проверяет, зарегистрирован ли телефон.
     * @param phone Проверяемый телефон.
     * @return true, если клиент с таким телефоном есть.
     */
    bool contains(const PhoneNumber& phone) const;

    /**
     * @brief Находит клиента по телефону.
     * @param phone Телефон клиента.
     * @return Найденный клиент.
     * @throws ClientNotFoundException Если клиент не зарегистрирован.
     */
    std::shared_ptr<Client> findByPhone(const PhoneNumber& phone) const;

    /**
     * @brief Возвращает клиента по порядковому номеру.
     * @param index Номер, начиная с нуля.
     * @return Клиент.
     * @throws NotFoundException Если номер выходит за границы.
     */
    std::shared_ptr<Client> clientAt(std::size_t index) const;

    /**
     * @brief Собирает постоянных клиентов.
     * @param requiredOrders Число заказов, после которого клиент постоянный.
     * @return Список постоянных клиентов.
     */
    std::vector<std::shared_ptr<Client>> regularClients(int requiredOrders) const;

private:
    std::vector<std::shared_ptr<Client>> clients_;
};

}
